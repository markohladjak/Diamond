/*
 * ESP32MeshNetService.h
 *
 *  Created on: Mar 19, 2021
 *      Author: rem
 */

#ifndef NET_ESP32MESHNETSERVICE_H_
#define NET_ESP32MESHNETSERVICE_H_

#include <Net/INetService.h>
#include "esp_mesh.h"
#include "Net/FA/SystemUpdate.h"


#include <map>

namespace diamon {

typedef void (*_layer_changed_callback_funct_t)(int);
typedef void (*_is_root_callback_funct_t)();

class ESP32MeshNetService: public INetService, public INetwork {
	static QueueHandle_t _send_mutex;
	// Mesh
	static String _router_ssid;
	static String _router_pw;
	static int	_router_chennel;

	static String _mesh_pw;
	static const uint8_t MESH_ID[6];

	static bool _is_mesh_initialized;
	static bool _is_mesh_connected;
	static int _max_device_count;
	static int _max_connection;

	static int _mesh_layer;
	static mesh_addr_t _mesh_parent_addr;

	static uint64_t _node_addr_mask;

	static bool _is_running;

	static std::map<NetAddress, ESP32MeshNetService*> _local;

	static _layer_changed_callback_funct_t _layer_changed_callback_funct;
	static _is_root_callback_funct_t _is_root_callback_funct;

	static void init();
	static void deinit();

	static esp_err_t system_event_handler(void *ctx, system_event_t *event);

	static void mesh_event_handler(mesh_event_t event);
	static void mesh_scan_done_handler(int num);
	static void mesh_data_rxtx_start();
	static void mesh_rx_task(void *arg);
	static void mesh_send_task(void *arg);

	static void run_mdns();

	static void on_this_became_root();
	static void on_layer_changed(int layer);
	static void on_routing_table_changed();
	static void on_root_address();

	static uint64_t self_address(uint16_t device_num);

	static void print_route_table();

	static void wifi_scan(void);

public:
	NetAddress Address;

	TEvent<int> OnLayerChanged;
	TEvent<> OnNodeIsRootEvent;

	static bool _is_root;
	static bool _is_root_fixed;

	ESP32MeshNetService(uint16_t device_num);
	virtual ~ESP32MeshNetService();

	static void Init();
	static void Start();

	void Send(NetMessage &msg, NetAddress to) override;
	void Send(CommunicationData* data, NetAddress to) override;

	void OnReceive(NetAddress from,	void (*onReceive)(NetMessage&)) override;
	void setWIFIMode(String ssid, String pw) override;

	std::list<NetAddress> GetConnectedDevices() override;

	void GetAPList() override;


	static void OnLayerChangedCallbackRegister(_layer_changed_callback_funct_t funct);
	static void OnIsRootCallbackRegister(_is_root_callback_funct_t funct);


	data_receive_callback_t _binary_callback_funct ;

	NetAddressList devices_address_list() override;
	void receive_callback_register(data_receive_callback_t callback_funct) override;
	int tttsend(uint8_t *data, int lenght) override;


private:
	void send_msg(NetMessage &msg, NetAddress to);
};

} /* namespace diamon */

#endif /* NET_ESP32MESHNETSERVICE_H_ */
