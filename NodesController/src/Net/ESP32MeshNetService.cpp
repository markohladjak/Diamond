/*
 * ESP32MeshNetService.cpp
 *
 *  Created on: Mar 19, 2021
 *      Author: rem
 */


#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mesh_internal.h"
#include "nvs_flash.h"
#include <Net/ESP32MeshNetService.h>
#include <LogService.h>
#include <ESPmDNS.h>

namespace diamon {

#define ESP_LOGI(tag, ...) Serial.printf(__VA_ARGS__)
#define ESP_LOGW(tag, ...) Serial.printf(__VA_ARGS__)


QueueHandle_t ESP32MeshNetService::_send_mutex = xSemaphoreCreateMutex();


//String ESP32MeshNetService::_router_ssid = "TP-LINK_18";
//String ESP32MeshNetService::_router_pw = "13643522";
//String ESP32MeshNetService::_router_ssid = "Tenda_0AD898";
//String ESP32MeshNetService::_router_pw = "12345678";
//String ESP32MeshNetService::_router_ssid = "TP-Link_982C";
//String ESP32MeshNetService::_router_pw = "33188805";
//String ESP32MeshNetService::_router_ssid = "HUAWEI MediaPad T5";
//String ESP32MeshNetService::_router_pw = "123321123";
String ESP32MeshNetService::_router_ssid = "DiamandNC";
String ESP32MeshNetService::_router_pw = "nodesAdmin";

int	ESP32MeshNetService::_router_chennel = 1;

String ESP32MeshNetService::_mesh_pw = "34578456";

bool ESP32MeshNetService::_is_mesh_connected = false;
bool ESP32MeshNetService::_is_mesh_initialized = false;

int ESP32MeshNetService::_max_device_count = 100;
int ESP32MeshNetService::_max_connection = 10;  // 10 MAX

int ESP32MeshNetService::_mesh_layer = -1;
mesh_addr_t ESP32MeshNetService::_mesh_parent_addr;
uint64_t ESP32MeshNetService::_node_addr_mask = 0;

const uint8_t ESP32MeshNetService::MESH_ID[6] = { 0x81, 0x81, 0x81, 0x81, 0x81, 0x81 };

_layer_changed_callback_funct_t ESP32MeshNetService::_layer_changed_callback_funct = NULL;
_is_root_callback_funct_t ESP32MeshNetService::_is_root_callback_funct = NULL;

bool ESP32MeshNetService::_is_running = false;

bool ESP32MeshNetService::_is_root = false;
bool ESP32MeshNetService::_is_root_fixed = true;

std::map<NetAddress, ESP32MeshNetService*> ESP32MeshNetService::_local;

#define RX_SIZE          (1500)
#define TX_SIZE          (1460)

static uint8_t tx_buf[TX_SIZE] = { 0, };
static uint8_t rx_buf[RX_SIZE] = { 0, };

bool is_rx_running = false;

//WIFI_REASON_CIPHER_SUITE_REJECTED

ESP32MeshNetService::ESP32MeshNetService(uint16_t device_num) {
	Address = self_address(device_num);

	_local[Address] = this;
}

ESP32MeshNetService::~ESP32MeshNetService() {
	_local.erase(Address);

	if (!_local.size() && _is_mesh_initialized)
		deinit();
}

void ESP32MeshNetService::Init() {
	if (!_is_mesh_initialized)
		init();
}

void ESP32MeshNetService::Start() {
	if (!_is_mesh_initialized) {
	    ESP_LOGI(MESH_TAG, "mesh not initialized\n");

	    return;
	}

	ESP_ERROR_CHECK(esp_mesh_start());
    ESP_LOGI(MESH_TAG, "mesh starts successfully, heap:%d, %s\n",  esp_get_free_heap_size(),
             esp_mesh_is_root_fixed() ? "root fixed" : "root not fixed");

    print_route_table();
}

void ESP32MeshNetService::Send(NetMessage &msg, NetAddress to) {
	xSemaphoreTake(_send_mutex, portMAX_DELAY);

	LogService::Log("ESP32MeshNetService::Send", msg);

	esp_err_t err;
    mesh_addr_t route_table[_max_device_count];
    int route_table_size = 0;
    mesh_data_t data;
    data.data = tx_buf;
    data.size = sizeof(tx_buf);
    data.proto = MESH_PROTO_JSON;
    data.tos = MESH_TOS_P2P;
    bool to_server = false;

	msg._from = Address;
	msg._to = to;

//    if (to == NetAddress::BROADCAST)
//    	esp_mesh_get_routing_table((mesh_addr_t *) &route_table, _max_device_count * 6, &route_table_size);
//    else
    if (to == NetAddress::SERVER) {
    	route_table_size = 1;

    	to_server = true;
    }
    else {
    	uint64_t addr = (uint64_t)to;
    	uint8_t *mac = ((uint8_t*)&addr) + 2;

    	memcpy(route_table[0].addr, mac, 6);

    	route_table_size = 1;
    }

    String json = msg;

	memcpy(tx_buf, json.c_str(), json.length());
	data.size = json.length();

	for (int i = 0; i < route_table_size; ++i) {
		printf(">%3d    " MACSTR "\n", i, MAC2STR(route_table[i].addr));

		err = esp_mesh_send(to_server ? NULL : &route_table[i], &data, MESH_DATA_P2P, NULL, 0);
		if (err) {
			ESP_LOGE(MESH_TAG,
					 "[ROOT-2-UNICAST:%d][L:%d]parent:" MACSTR " to " MACSTR ", heap:%d[err:0x%x, proto:%d, tos:%d]",
					 send_count, mesh_layer, MAC2STR(mesh_parent_addr.addr),
					 MAC2STR(route_table[i].addr), esp_get_minimum_free_heap_size(),
					 err, data.proto, data.tos);
		}
	}

	xSemaphoreGive(_send_mutex);
}

void ESP32MeshNetService::OnReceive(NetAddress from, void (*onReceive)(NetMessage&)) {


    }

void ESP32MeshNetService::setWIFIMode(diamon::WIFIMODE mode, String ssid, String pw) {
}

std::list<NetAddress> ESP32MeshNetService::GetConnectedDevices() {
	return std::list<NetAddress>();
}

void ESP32MeshNetService::OnLayerChangedCallbackRegister(_layer_changed_callback_funct_t funct) {
	_layer_changed_callback_funct = funct;
}

void ESP32MeshNetService::OnIsRootCallbackRegister(_is_root_callback_funct_t funct){
	_is_root_callback_funct = funct;
}

void ESP32MeshNetService::init() {
	//    esp_log_level_set("*", esp_log_level_t::ESP_LOG_VERBOSE);

	//    ESP_ERROR_CHECK(mesh_light_init());
	    ESP_ERROR_CHECK(nvs_flash_init());
	    /*  tcpip initialization */
	    tcpip_adapter_init();
	    /* for mesh
	     * stop DHCP server on softAP interface by default
	     * stop DHCP client on station interface by default
	     * */
	    ESP_ERROR_CHECK(tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP));
	    ESP_ERROR_CHECK(tcpip_adapter_dhcpc_stop(TCPIP_ADAPTER_IF_STA));


	    if (_is_root) {
			/* static ip settings */
			tcpip_adapter_ip_info_t sta_ip;
			sta_ip.ip.addr = ipaddr_addr("10.10.10.10");
			sta_ip.gw.addr = ipaddr_addr("10.10.10.1");
			sta_ip.netmask.addr = ipaddr_addr("255.255.255.0");
			tcpip_adapter_set_ip_info(tcpip_adapter_if_t::TCPIP_ADAPTER_IF_STA, &sta_ip);
	    }

	    /*  wifi initialization */
	    ESP_ERROR_CHECK(esp_event_loop_init(NULL, NULL));
	    wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
	    ESP_ERROR_CHECK(esp_wifi_init(&config));
	    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	    ESP_ERROR_CHECK(esp_wifi_start());
	    /*  mesh initialization */
	    ESP_ERROR_CHECK(esp_mesh_init());
//	    ESP_ERROR_CHECK(esp_mesh_set_max_layer(4));
	    ESP_ERROR_CHECK(esp_mesh_set_vote_percentage(1));
	    ESP_ERROR_CHECK(esp_mesh_set_ap_assoc_expire(10));


	    if (_is_root_fixed)
	    	ESP_ERROR_CHECK(esp_mesh_fix_root(1));

	    mesh_cfg_t cfg;

	    memset(&cfg, 0, sizeof(mesh_cfg_t));

	    cfg.crypto_funcs = &g_wifi_default_mesh_crypto_funcs;
	    cfg.allow_channel_switch = false;

	    memcpy((uint8_t *) &cfg.mesh_id, MESH_ID, 6);
	    cfg.event_cb = &mesh_event_handler;
	    cfg.channel = _router_chennel;

	    cfg.router.ssid_len = _router_ssid.length();
	    memcpy((uint8_t *) &cfg.router.ssid, _router_ssid.c_str(), cfg.router.ssid_len);
	    memcpy((uint8_t *) &cfg.router.password, _router_pw.c_str(), _router_pw.length());

	    /* mesh softAP */
	    ESP_ERROR_CHECK(esp_mesh_set_ap_authmode(wifi_auth_mode_t::WIFI_AUTH_WPA_WPA2_PSK));
	    cfg.mesh_ap.max_connection = _max_connection;
	    memcpy((uint8_t *) &cfg.mesh_ap.password, _mesh_pw.c_str(), _mesh_pw.length());

	    ESP_ERROR_CHECK(esp_mesh_set_config(&cfg));

	    if (_is_root) {
	    	wifi_config_t parent_cfg;

		    memset(&parent_cfg, 0, sizeof(wifi_config_t));

		    parent_cfg.sta.bssid_set = false;
		    parent_cfg.sta.channel = _router_chennel;
//		    parent_cfg.sta.bssid = { 0xc8, 0x3a, 0x35, 0x0a, 0xd8, 0x98 };
//		    *((uint32_t*)parent_cfg.sta.bssid) = 0x0a353ac8;
//		    *((uint16_t*)(parent_cfg.sta.bssid + 4)) = 0x98d8;

		    memcpy((uint8_t *) &parent_cfg.sta.ssid, _router_ssid.c_str(), _router_ssid.length());
		    memcpy((uint8_t *) &parent_cfg.sta.password, _router_pw.c_str(), _router_pw.length());

//			ESP_ERROR_CHECK(esp_mesh_set_ap_authmode(wifi_auth_mode_t::WIFI_AUTH_WPA_WPA2_PSK));

	    	esp_mesh_set_parent(&parent_cfg, NULL, mesh_type_t::MESH_ROOT, MESH_ROOT_LAYER);
	    }

	    _is_mesh_initialized = true;
}

void ESP32MeshNetService::deinit() {
///// ToDO
}

void ESP32MeshNetService::mesh_event_handler(mesh_event_t event)
{
    mesh_addr_t id = {0,};
    static uint8_t last_layer = 0;
    ESP_LOGD(MESH_TAG, "esp_event_handler:%d\n", event.id);

    switch (event.id) {
    case MESH_EVENT_STARTED:
        esp_mesh_get_id(&id);
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_STARTED>ID:" MACSTR "\n", MAC2STR(id.addr));
        _is_mesh_connected = false;
        _mesh_layer = esp_mesh_get_layer();
        break;
    case MESH_EVENT_STOPPED:
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_STOPPED>");
        _is_mesh_connected = false;
        _mesh_layer = esp_mesh_get_layer();
        break;
    case MESH_EVENT_CHILD_CONNECTED:
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_CHILD_CONNECTED>aid:%d, " MACSTR "\n",
                 event.info.child_connected.aid,
                 MAC2STR(event.info.child_connected.mac));
        break;
    case MESH_EVENT_CHILD_DISCONNECTED:
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_CHILD_DISCONNECTED>aid:%d, " MACSTR "\n",
                 event.info.child_disconnected.aid,
                 MAC2STR(event.info.child_disconnected.mac));
        break;
    case MESH_EVENT_ROUTING_TABLE_ADD:
        ESP_LOGW(MESH_TAG, "<MESH_EVENT_ROUTING_TABLE_ADD>add %d, new:%d\n",
                 event.info.routing_table.rt_size_change,
                 event.info.routing_table.rt_size_new);

        on_routing_table_changed();
        break;
    case MESH_EVENT_ROUTING_TABLE_REMOVE:
        ESP_LOGW(MESH_TAG, "<MESH_EVENT_ROUTING_TABLE_REMOVE>remove %d, new:%d\n",
                 event.info.routing_table.rt_size_change,
                 event.info.routing_table.rt_size_new);

        on_routing_table_changed();
        break;
    case MESH_EVENT_NO_PARENT_FOUND:
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_NO_PARENT_FOUND>scan times:%d\n",
                 event.info.no_parent.scan_times);
        /* TODO handler for the failure */
        break;
    case MESH_EVENT_PARENT_CONNECTED:
        esp_mesh_get_id(&id);
        _mesh_layer = event.info.connected.self_layer;
        memcpy(&_mesh_parent_addr.addr, event.info.connected.connected.bssid, 6);
        ESP_LOGI(MESH_TAG,
                 "<MESH_EVENT_PARENT_CONNECTED>layer:%d-->%d, parent:" MACSTR "%s, ID:" MACSTR "\n",
                 last_layer, _mesh_layer, MAC2STR(_mesh_parent_addr.addr),
                 esp_mesh_is_root() ? "<ROOT>" :
                 (_mesh_layer == 2) ? "<layer2>" : "", MAC2STR(id.addr));
        last_layer = _mesh_layer;
        _is_mesh_connected = true;

        on_layer_changed(_mesh_layer);

        mesh_data_rxtx_start();

//		if (esp_mesh_is_root()) {
//			tcpip_adapter_dhcpc_start(TCPIP_ADAPTER_IF_STA);
//		}
//        esp_mesh_comm_p2p_start();
        break;
    case MESH_EVENT_PARENT_DISCONNECTED:
        ESP_LOGI(MESH_TAG,
                 "<MESH_EVENT_PARENT_DISCONNECTED>reason:%d\n",
                 event.info.disconnected.reason);
        _is_mesh_connected = false;
        on_layer_changed(-1);
        _mesh_layer = esp_mesh_get_layer();
        break;
    case MESH_EVENT_LAYER_CHANGE:
        _mesh_layer = event.info.layer_change.new_layer;
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_LAYER_CHANGE>layer:%d-->%d%s\n",
                 last_layer, _mesh_layer,
                 esp_mesh_is_root() ? "<ROOT>" :
                 (_mesh_layer == 2) ? "<layer2>" : "");
        last_layer = _mesh_layer;
        on_layer_changed(_mesh_layer);
        break;
    case MESH_EVENT_ROOT_ADDRESS:
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_ROOT_ADDRESS>root address:" MACSTR "\n",
                 MAC2STR(event.info.root_addr.addr));

        if (esp_mesh_is_root())
        	on_this_became_root();

        on_root_address();
        break;
    case MESH_EVENT_ROOT_GOT_IP:
        /* root starts to connect to server */

        ESP_LOGI(MESH_TAG,
                 "<MESH_EVENT_ROOT_GOT_IP>sta ip: " IPSTR ", mask: " IPSTR ", gw: " IPSTR "\n",
                 IP2STR(&event.info.got_ip.ip_info.ip),
                 IP2STR(&event.info.got_ip.ip_info.netmask),
                 IP2STR(&event.info.got_ip.ip_info.gw));

        if (esp_mesh_is_root()) {
        	run_mdns();
        }
        break;
    case MESH_EVENT_ROOT_LOST_IP:
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_ROOT_LOST_IP>");
        break;
    case MESH_EVENT_VOTE_STARTED:
        ESP_LOGI(MESH_TAG,
                 "<MESH_EVENT_VOTE_STARTED>attempts:%d, reason:%d, rc_addr:" MACSTR "\n",
                 event.info.vote_started.attempts,
                 event.info.vote_started.reason,
                 MAC2STR(event.info.vote_started.rc_addr.addr));
        break;
    case MESH_EVENT_VOTE_STOPPED:
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_VOTE_STOPPED>");
        break;
    case MESH_EVENT_ROOT_SWITCH_REQ:
        ESP_LOGI(MESH_TAG,
                 "<MESH_EVENT_ROOT_SWITCH_REQ>reason:%d, rc_addr:" MACSTR "\n",
                 event.info.switch_req.reason,
                 MAC2STR( event.info.switch_req.rc_addr.addr));
        break;
    case MESH_EVENT_ROOT_SWITCH_ACK:
        /* new root */
        _mesh_layer = esp_mesh_get_layer();
        esp_mesh_get_parent_bssid(&_mesh_parent_addr);
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_ROOT_SWITCH_ACK>layer:%d, parent:" MACSTR "\n", _mesh_layer, MAC2STR(_mesh_parent_addr.addr));
        break;
    case MESH_EVENT_TODS_STATE:
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_TODS_REACHABLE>state:%d\n",
                 event.info.toDS_state);
        break;
    case MESH_EVENT_ROOT_FIXED:
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_ROOT_FIXED>%s\n",
                 event.info.root_fixed.is_fixed ? "fixed" : "not fixed");
        break;
    case MESH_EVENT_ROOT_ASKED_YIELD:
        ESP_LOGI(MESH_TAG,
                 "<MESH_EVENT_ROOT_ASKED_YIELD>" MACSTR ", rssi:%d, capacity:%d\n",
                 MAC2STR(event.info.root_conflict.addr),
                 event.info.root_conflict.rssi,
                 event.info.root_conflict.capacity);
        break;
    case MESH_EVENT_CHANNEL_SWITCH:
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_CHANNEL_SWITCH>new channel:%d\n", event.info.channel_switch.channel);
        break;
    case MESH_EVENT_SCAN_DONE:
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_SCAN_DONE>number:%d\n",
                 event.info.scan_done.number);

//	    if (_is_root)
//	    	mesh_scan_done_handler(event.info.scan_done.number);
        break;
    case MESH_EVENT_NETWORK_STATE:
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_NETWORK_STATE>is_rootless:%d\n",
                 event.info.network_state.is_rootless);
        break;
    case MESH_EVENT_STOP_RECONNECTION:
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_STOP_RECONNECTION>\n");
        break;
    case MESH_EVENT_FIND_NETWORK:
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_FIND_NETWORK>new channel:%d, router BSSID:" MACSTR "\n",
                 event.info.find_network.channel, MAC2STR(event.info.find_network.router_bssid));
        break;
    case MESH_EVENT_ROUTER_SWITCH:
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_ROUTER_SWITCH>new router:%s, channel:%d, " MACSTR "\n",
                 event.info.router_switch.ssid, event.info.router_switch.channel, MAC2STR(event.info.router_switch.bssid));
        break;
    default:
        ESP_LOGI(MESH_TAG, "unknown id:%d\n", event.id);
        break;
    }
}

void ESP32MeshNetService::mesh_data_rxtx_start() {
    static bool is_rxtx_started = false;
    if (!is_rxtx_started) {
    	is_rxtx_started = true;
        xTaskCreate(mesh_rx_task, "MPRX", 3072, NULL, 5, NULL);
    }
}

void ESP32MeshNetService::mesh_scan_done_handler(int num)
{
	int i;
	int ie_len = 0;
	mesh_assoc_t assoc;
	mesh_assoc_t parent_assoc;
	wifi_ap_record_t record;
	wifi_ap_record_t parent_record = { 0, };
	bool parent_found = false;
	mesh_type_t my_type = MESH_ROOT;
	int my_layer = MESH_ROOT_LAYER;
	wifi_config_t parent = { 0, };
	wifi_scan_config_t scan_config = { 0 };

	parent_assoc.layer = 25;
	parent_assoc.rssi = -120;

	for (i = 0; i < num; i++) {
		esp_mesh_scan_get_ap_ie_len(&ie_len);
		esp_mesh_scan_get_ap_record(&record, &assoc);
		if (ie_len == sizeof(assoc)) {
			ESP_LOGW(MESH_TAG,
					 "<MESH>[%d]%s, layer:%d/%d, assoc:%d/%d, %d, "MACSTR", channel:%u, rssi:%d, ID<"MACSTR"><%s>",
					 i, record.ssid, assoc.layer, assoc.layer_cap, assoc.assoc,
					 assoc.assoc_cap, assoc.layer2_cap, MAC2STR(record.bssid),
					 record.primary, record.rssi, MAC2STR(assoc.mesh_id), assoc.encrypted ? "IE Encrypted" : "IE Unencrypted");

		} else {
			ESP_LOGI(MESH_TAG, "[%d]%s, "MACSTR", channel:%u, rssi:%d", i,
					 record.ssid, MAC2STR(record.bssid), record.primary,
					 record.rssi);

			if (!strcmp(_router_ssid.c_str(), (char *) record.ssid)) {
				parent_found = true;
				memcpy(&parent_record, &record, sizeof(record));
			}
		}
	}

	esp_mesh_flush_scan_result();

	if (parent_found) {
		/*
		 * parent
		 * Both channel and SSID of the parent are mandatory.
		 */
		parent.sta.channel = parent_record.primary;
		memcpy(&parent.sta.ssid, &parent_record.ssid, sizeof(parent_record.ssid));
		parent.sta.bssid_set = 1;
		memcpy(&parent.sta.bssid, parent_record.bssid, 6);
		ESP_ERROR_CHECK(esp_mesh_set_ap_authmode(parent_record.authmode));

		if (parent_record.authmode != WIFI_AUTH_OPEN) {
			memcpy(&parent.sta.password, _router_pw.c_str(), _router_pw.length());
		}

		ESP_LOGW(MESH_TAG, "<PARENT>%s, "MACSTR", channel:%u, rssi:%d",
				 parent_record.ssid, MAC2STR(parent_record.bssid),
				 parent_record.primary, parent_record.rssi);

		ESP_ERROR_CHECK(esp_mesh_set_parent(&parent, (mesh_addr_t *)&parent_assoc.mesh_id, my_type, my_layer));

	}
}

//void ESP32MeshNetService::mesh_send_task(void *arg)
//{
//    int i;
//    esp_err_t err;
//    int send_count = 0;
//    mesh_addr_t route_table[_max_device_count];
//    int route_table_size = 0;
//    mesh_data_t data;
//    data.data = tx_buf;
//    data.size = sizeof(tx_buf);
//    data.proto = MESH_PROTO_BIN;
//    data.tos = MESH_TOS_P2P;
//
//    while (_is_running) {
//        /* non-root do nothing but print */
//        if (!esp_mesh_is_root()) {
//            ESP_LOGI(MESH_TAG, "layer:%d, rtableSize:%d, %s", _mesh_layer,
//                     esp_mesh_get_routing_table_size(),
//                     (_is_mesh_connected && esp_mesh_is_root()) ? "ROOT" : _is_mesh_connected ? "NODE" : "DISCONNECT");
//            vTaskDelay(10 * 1000 / portTICK_RATE_MS);
//            continue;
//        }
//        esp_mesh_get_routing_table((mesh_addr_t *) &route_table, _max_device_count * 6, &route_table_size);
//        if (send_count && !(send_count % 100)) {
//            ESP_LOGI(MESH_TAG, "size:%d/%d,send_count:%d", route_table_size,
//                     esp_mesh_get_routing_table_size(), send_count);
//        }
//        send_count++;
//        tx_buf[25] = (send_count >> 24) & 0xff;
//        tx_buf[24] = (send_count >> 16) & 0xff;
//        tx_buf[23] = (send_count >> 8) & 0xff;
//        tx_buf[22] = (send_count >> 0) & 0xff;
//        if (send_count % 2) {
//            memcpy(tx_buf, (uint8_t *)&light_on, sizeof(light_on));
//        } else {
//            memcpy(tx_buf, (uint8_t *)&light_off, sizeof(light_off));
//        }
//
//        for (i = 0; i < route_table_size; i++) {
//            err = esp_mesh_send(&route_table[i], &data, MESH_DATA_P2P, NULL, 0);
//            if (err) {
//                ESP_LOGE(MESH_TAG,
//                         "[ROOT-2-UNICAST:%d][L:%d]parent:"MACSTR" to "MACSTR", heap:%d[err:0x%x, proto:%d, tos:%d]",
//                         send_count, mesh_layer, MAC2STR(mesh_parent_addr.addr),
//                         MAC2STR(route_table[i].addr), esp_get_minimum_free_heap_size(),
//                         err, data.proto, data.tos);
//            } else if (!(send_count % 100)) {
//                ESP_LOGW(MESH_TAG,
//                         "[ROOT-2-UNICAST:%d][L:%d][rtableSize:%d]parent:"MACSTR" to "MACSTR", heap:%d[err:0x%x, proto:%d, tos:%d]",
//                         send_count, mesh_layer,
//                         esp_mesh_get_routing_table_size(),
//                         MAC2STR(mesh_parent_addr.addr),
//                         MAC2STR(route_table[i].addr), esp_get_minimum_free_heap_size(),
//                         err, data.proto, data.tos);
//            }
//        }
//        /* if route_table_size is less than 10, add delay to avoid watchdog in this task. */
//        if (route_table_size < 10) {
//            vTaskDelay(1 * 1000 / portTICK_RATE_MS);
//        }
//    }
//    vTaskDelete(NULL);
//}

void ESP32MeshNetService::mesh_rx_task(void *args) {
	static bool is_rx_running = false;
	if (is_rx_running) {
		LogService::Log("ESP32MeshNetService::mesh_rx_task", "already started");

		return;
	}

    int recv_count = 0;
    esp_err_t err;
    mesh_addr_t from;
    int send_count = 0;
    mesh_data_t data;
    int flag = 0;
    data.data = rx_buf;
    data.size = RX_SIZE;
    is_rx_running = true;

    while (is_rx_running) {
        data.size = RX_SIZE;
        err = esp_mesh_recv(&from, &data, portMAX_DELAY, &flag, NULL, 0);
        if (err != ESP_OK || !data.size) {
            ESP_LOGE(MESH_TAG, "err:0x%x, size:%d", err, data.size);
            continue;
        }

        data.data[data.size] = 0;

        LogService::Log("ESP32MeshNetService::mesh_rx_task", (char*)data.data);

        auto msg = NetMessage::Resolve((char*)data.data);

        if (msg->_to == NetAddress::BROADCAST) {
        	for (auto dvs: _local)
        		if (dvs.first != msg->_from)
        			dvs.second->OnReceiveEvent(msg->_from, msg);

        	delete msg;
        	continue;
        }

        if (msg->_to == NetAddress::SERVER)
        	msg->_to = _node_addr_mask + msg->_to;

        auto dest = _local.find(msg->_to);
        if (dest != _local.end()){
        	dest->second->OnReceiveEvent(msg->_from, msg);
        }

    	delete msg;
    }

    is_rx_running = false;

    vTaskDelete(NULL);
}

void ESP32MeshNetService::run_mdns() {
	static bool started = false;

	if (started)
		return;
	started = true;

	if (!MDNS.begin("diamond")) {
		LogService::Log("MDNS", "Error setting up MDNS responder!");
	} else
		LogService::Log("MDNS", "Started.");
}

void ESP32MeshNetService::on_this_became_root() {
	if (_is_root_callback_funct)
		_is_root_callback_funct();

	for (auto netdvs: _local)
		netdvs.second->OnNodeIsRootEvent();
}

void ESP32MeshNetService::on_layer_changed(int layer){
	if (_layer_changed_callback_funct)
		_layer_changed_callback_funct(layer);

	for (auto netdvs: _local)
		netdvs.second->OnLayerChanged(layer);
}

void ESP32MeshNetService::on_routing_table_changed(){
	print_route_table();

	for (auto netdvs: _local)
		netdvs.second->OnLayoutChangedEvent();
}

void ESP32MeshNetService::on_root_address() {
	for (auto netdvs: _local)
		netdvs.second->OnRootAddressObtainedEvent();
}

uint64_t ESP32MeshNetService::self_address(uint16_t device_num) {
    mesh_addr_t route_table[1];
    int route_table_size = 0;

    if (!_node_addr_mask) {
		esp_mesh_get_routing_table((mesh_addr_t *) &route_table, 6, &route_table_size);

		_node_addr_mask = *((uint64_t*)(route_table[0].addr)) << 16;

		_node_addr_mask &= 0xFFFFFFFFFFFF0000;
    }

	return _node_addr_mask + device_num;
}

void ESP32MeshNetService::print_route_table() {
    mesh_addr_t route_table[_max_device_count];
    int route_table_size = 0;

	esp_mesh_get_routing_table((mesh_addr_t *) &route_table, _max_device_count * 6, &route_table_size);

	printf("Routing table:\n");

	for (int i = 0; i < route_table_size; ++i) {
		printf("%3d    " MACSTR "\n", i, MAC2STR(route_table[i].addr));
	}
}

} /* namespace diamon */
