var sse;
var SelectedDeviceID;

const MAX_DEVICE_NAME_LENGHT = 16;

function serverSendInit() {
	if (!!window.EventSource) {
		sse = new EventSource('/events');

        sse.addEventListener('open', function (e) {
          console.log("Events Connected");
        }, false);

        sse.addEventListener('error', function (e) {
          if (e.target.readyState != EventSource.OPEN) {
            console.log("Events Disconnected");
          }
        }, false);

        sse.addEventListener('message', function (e) {
          console.log("message", e.data);
        }, false);

        sse.addEventListener('msg', function (e) {
          console.log("msg", e.data);
          processMessage(e.data);
        }, false);

        sse.addEventListener('state', function (e) {
          console.log("state", e.data);

          refreshAll(e.data);
        }, false);
	}
}

function processMessage(msg) {
  	var m = JSON.parse(msg) 
	console.log(m);

    switch (m.command) {
      case "add": addDevice({ 'id': m.id, 'state': m.state, 'version': m.version });
        break;
      case "rem": removeDevice(m.id);
        break;
      case "set": setDeviceInfo({ 'id': m.id, 'state': m.state });
        break;
    }
}

function addDevice(dvInfo) {
	var devicesList = document.querySelector('#devicesList');
	
	var sequence = devicesList.childNodes.length;
	
	devicesList.appendChild(createDeviceView(dvInfo, sequence));

  	console.log("addDevice", dvInfo.id, dvInfo.name, dvInfo.state, dvInfo.version);
}

function removeDevice(id) {
  	document.getElementById(id).remove();
}

function setDeviceInfo(info){
  	var dv = document.getElementById(info.id);

	var dvStatus = dv.getElementsByClassName("deviceStatus")[0];
	var statusText = dv.getElementsByClassName("deviceStatusText")[0];

	dvStatus.className = dvStatus.className.replace(/dvs[A-Z]+/g, "dvs" + info.state.toUpperCase());
				
	statusText.textContent = info.state;
}

function refreshAll(data) {
	console.log("refreshAll", data);
	
	var devicesList = document.querySelector('#devicesList');

	devicesList.innerHTML = '';
	
  	var m = JSON.parse(data); 
	
	var ids = Object.keys(m);
	
	for (var i=0; i < ids.length; i++) 
		addDevice( { 'id': ids[i], 'state': m[ids[i]].state, 'name': m[ids[i]].name, 'version': m[ids[i]].version } );
}

function openTab(evt, tabName) {
	var i, tabcontent, tablinks;
	tabcontent = document.getElementsByClassName("tabContent");
	for (i = 0; i < tabcontent.length; i++) {
		tabcontent[i].style.display = "none";
	}
	tablinks = document.getElementsByClassName("tablinks");
	for (i = 0; i < tablinks.length; i++) {
		tablinks[i].className = tablinks[i].className.replace(" active", "");
	}
	document.getElementById(tabName).style.display = "block";
	evt.currentTarget.className += " active";
}
		
function createDeviceView(info, sequence) {
	var id = info.id;
	var name = info.name;
	var state = info.state;
	var is_connected = info.is_connected;
	var version = info.version;
	
	var template = document.querySelector('#deviceViewTemplate');
				
	var dv = template.content.querySelector("div");
	var deviceView = document.importNode(dv, true);
	
	var deviceName = deviceView.getElementsByClassName("deviceName")[0];
	var status = deviceView.getElementsByClassName("deviceStatus")[0];
	var statusText = deviceView.getElementsByClassName("deviceStatusText")[0];
	var deviceVersionText = deviceView.getElementsByClassName("deviceVersionText")[0];


	//if(firstEL++ == 0)			deviceName.classList.add("editName");
	
	if (is_connected == false)
		deviceView.classList.add("deviceDisconnected");
	else
		deviceView.classList.remove("deviceDisconnected");

	//			let eee = new Map([
	//  				['Free', 'Free'],
	//  				[1, 'one'],
	//  				[1, 'one'],
	//			]);
	
	deviceView.id = id;
	deviceView.classList.add(sequence % 2 ? "darkBackground" : "lightBackground");
	
	deviceName.value = name;
	deviceName.name = name;
	deviceName.placeholder=id;
	deviceName.maxLength = MAX_DEVICE_NAME_LENGHT;
	
	status.classList.add("dvs" + state.toUpperCase());
	statusText.textContent = state;
	
	if (version)
		deviceVersionText.textContent = version;
	
	return deviceView; 
}
		 
function requestDeviceState(id, state) {
	var url = window.location.href + `request?type=state&id=${id}&state=${state}`;
		
	httpGet(url);
}

function reportAll()
{
	var url = window.location.href + `report_all`;

	httpGet(url);
}

function resetAll(state = "NONE")
{
	var url = window.location.href + `reset_all?`;

	httpGet(url);
}

function httpGet(theUrl) {
	var xmlHttp = new XMLHttpRequest();
	xmlHttp.open("GET", theUrl, true); // false for synchronous request
	xmlHttp.send(null);
	
	console.log("Request: ", url);

	return xmlHttp.responseText;
}

function changeWIFIMode() {
	var ssid = document.getElementById("wifi_ssid").value;
	var pass = document.getElementById("wifi_password").value;
	
	var url = window.location.href + `request?type=wifi&ssid=${ssid}&password=${pass}`;
	
	httpGet(url);
	
	console.log("changeWIFIMode:", url);
}

function openNav() {
	w = document.getElementById("mySidenav").style.width;
	document.getElementById("mySidenav").style.width = w == "0px" ? "250px" : "0px";
}

function closeNav() {
	document.getElementById("mySidenav").style.width = "0";
}

function offset(el) {
    var rect = el.getBoundingClientRect(),
    scrollLeft = window.pageXOffset || document.documentElement.scrollLeft,
    scrollTop = window.pageYOffset || document.documentElement.scrollTop;
    return { top: rect.top + scrollTop, left: rect.left + scrollLeft }
}
		
function showActionPanel(el) {
	var dv = el.parentElement;
	var menu = document.getElementById("actionPanel");
	var menuTitle = menu.getElementsByClassName("actionPanelTitle")[0];
	var checked = menu.getElementsByClassName("actionChecked");
	var dvStatus = dv.getElementsByClassName("deviceStatus")[0];
	
	SelectedDeviceID = dv.id;
	
	menuTitle.textContent = 'Node:      ' + SelectedDeviceID;
	
	var pos = offset(el);
				
	menu.style.top = (pos.top - 60) + "px"; 
	menu.style.left = pos.left + "px"; 
	
	
	for(var c = menu.firstElementChild; c != null; c = c.nextElementSibling){
		console.log("c", c);
		
		var icon = c.getElementsByClassName("checkedIcon")[0];
		
		console.log("list", dvStatus.classList);
		console.log("name", c.className);
		
		if (icon)
			if (dvStatus.classList.contains(c.className))
				icon.style.display = "block";
			else
				icon.style.display = "none";
		
		console.log("display", icon);
	}

	
	actionPanelVisible(true);
}

function onActionSelect(el) {
	var action = el.getElementsByClassName("actionText")[0].textContent;
	
	actionPanelVisible(false);			

	requestDeviceState(SelectedDeviceID, action);
}

function actionPanelVisible(visible) {
	var menu = document.getElementById("actionPanel");
	
	if (visible) {
		if (menu.hasAttribute("hidden"))
			menu.removeAttribute("hidden");
	}
	else 
		if (!menu.hasAttribute("hidden"))
			menu.setAttribute("hidden", "");
}

window.addEventListener("click", function(event) {
		//console.log("window.addEventListener");
		//actionPanelVisible(false);
});
				
		
serverSendInit();
		
//		window.onclick = 
var dv_info = { 'id' : "24c438bd9e7c0001", 'name' : "", 'state': "Free", 'is_connected' : true };

addDevice(dv_info);

dv_info = { 'id' : "24c438bd9e7c0001", 'name' : "", 'state': "Free", 'is_connected' : false };
addDevice(dv_info);

//		addDevice("1", "Free", "", false);
//		addDevice("2", "Busy", "");
//		addDevice("3", "SOS", "");
//		setDeviceState("0", "Ready");
//		removeDevice("1");

$('.deviceName').dblclick(function(){
	console.log('dbclick');
    alert("The paragraph was double-clicked.");

	this.setAttribute('disabled', 'true');
});
		
function OnDeviceNameKeyPress(event) {
	var target = event.target;
	
	if (event.keyCode == 13 || event.keyCode == 27)
    //alert(event.keyCode);
		//this.blur();
		target.blur();

	if (event.keyCode == 13 && target.value != target.name) ;
		RequestNewDeviceName(target.value);

    //alert(event.keyCode);
}

function OnDeviceNameEditFinished(event) {
	event.target.value = event.target.name;	
}

function RequestNewDeviceName(name) {
	
}
