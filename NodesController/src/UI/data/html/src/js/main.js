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
  	let m = JSON.parse(msg) 
	console.log(m);

    switch (m.command) {
      case "add": addDevice({ 'id': m.id, 'state': m.state, 'version': m.version });
        break;
      case "rem": removeDevice(m.id);
        break;
      case "set": setDeviceInfo({ 'id': m.id, 'state': m.state });
        break;
      case "set_name": setDeviceName({ 'id': m.id, 'name': m.name });
        break;
    }
}

function addDevice(dvInfo) {
	let devicesList = document.querySelector('#devicesList');
	
	let sequence = devicesList.childNodes.length;
	
	devicesList.appendChild(createDeviceView(dvInfo, sequence));

  	console.log("addDevice", dvInfo.id, dvInfo.name, dvInfo.state, dvInfo.version);

	UpdateScheme();
}

function removeDevice(id) {
  	document.getElementById(id).remove();
}

function setDeviceInfo(info) {
  	let dv = document.getElementById(info.id);

	let dvStatus = dv.getElementsByClassName("deviceStatus")[0];
	let statusText = dv.getElementsByClassName("deviceStatusText")[0];

	dvStatus.className = dvStatus.className.replace(/dvs[A-Z]+/g, "dvs" + info.state.toUpperCase());
				
	statusText.textContent = info.state;

	document.getElementById("mainScheme").contentWindow.setDeviceInfo(info);
}

function setDeviceName(info){
  	let dv = document.getElementById(info.id);

	let dvName = dv.getElementsByClassName("deviceName")[0];

	dvName.value = info.name;
	dvName.name = info.name;

	UpdateScheme();
}

function refreshAll(data) {
	console.log("refreshAll", data);
	
	let devicesList = document.querySelector('#devicesList');

	devicesList.innerHTML = '';
	
  	let m = JSON.parse(data); 
	
	let ids = Object.keys(m);
	
	for (let i=0; i < ids.length; i++) 
		addDevice( { 'id': ids[i], 'state': m[ids[i]].state, 'name': m[ids[i]].name, 'version': m[ids[i]].version } );

	UpdateScheme();
}

function openTab(evt, tabName) {
	let i, tabcontent, tablinks;
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
	let id = info.id;
	let name = info.name;
	let state = info.state;
	let is_connected = info.is_connected;
	let version = info.version;
	
	let template = document.querySelector('#deviceViewTemplate');

	let dv = template.content.querySelector("div");
	let deviceView = document.importNode(dv, true);

	let deviceName = deviceView.getElementsByClassName("deviceName")[0];
	let status = deviceView.getElementsByClassName("deviceStatus")[0];
	let statusText = deviceView.getElementsByClassName("deviceStatusText")[0];
	let deviceVersionText = deviceView.getElementsByClassName("deviceVersionText")[0];


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
	deviceName.placeholder = id;
	deviceName.maxLength = MAX_DEVICE_NAME_LENGHT;
	
	status.classList.add("dvs" + state.toUpperCase());
	statusText.textContent = state;
	
	if (version)
		deviceVersionText.textContent = version;
	
	return deviceView; 
}
		
function currentURL() {
	return location.protocol + '//' + location.host + location.pathname;
}

function requestDeviceState(id, state) {
	let url = currentURL() + `request?type=state&id=${id}&state=${state}`;
		
	httpGet(url);
}

function reportAll()
{
	let url = window.location.href + `report_all`;

	httpGet(url);
}

function resetAll(state = "NONE")
{
	let url = window.location.href + `reset_all?state=${state}`;

	httpGet(url);
}

function httpGet(url) {
	let xmlHttp = new XMLHttpRequest();
	xmlHttp.open("GET", url, true); // false for synchronous request
	xmlHttp.send(null);
	
	console.log("Request: ", url);

	return xmlHttp.responseText;
}

function changeWIFIMode() {
	let ssid = document.getElementById("wifi_ssid").value;
	let pass = document.getElementById("wifi_password").value;
	
	let url = window.location.href + `request?type=wifi&ssid=${ssid}&password=${pass}`;
	
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
    let rect = el.getBoundingClientRect(),
    scrollLeft = window.pageXOffset || document.documentElement.scrollLeft,
    scrollTop = window.pageYOffset || document.documentElement.scrollTop;
    return { top: rect.top + scrollTop, left: rect.left + scrollLeft }
}
		
function showActionPanel(el, event) {
	let self_element = document.body.contains(el);

	let dv = el.parentElement;
	let menu = document.getElementById("actionPanel");
	let menuTitle = menu.getElementsByClassName("actionPanelTitle")[0];
	let dvStatus = dv.getElementsByClassName("deviceStatus")[0];
	
	SelectedDeviceID = dv.id;
	
	menuTitle.textContent = "Node: " + SelectedDeviceID;
	
	let pos = offset(el);
	let posX = event.pageX;
	let posY = event.pageY;

	if (!self_element) {
		posX += mainScheme.offsetLeft;
		posY += mainScheme.offsetTop;
	}
				
	// menu.style.top = (pos.top - 60) + "px"; 
	// menu.style.left = pos.left + "px"; 
	menu.style.top = (posY - 60) + "px"; 
	menu.style.left = posX + "px"; 
	
	
	for(let c = menu.firstElementChild; c != null; c = c.nextElementSibling){
		console.log("c", c);
		
		let icon = c.getElementsByClassName("checkedIcon")[0];
		
		console.log("list", dvStatus.classList);
		console.log("name", c.className);
		
		if (icon)
			if (dvStatus.classList.contains(c.className))
				icon.style.display = "block";
			else
				icon.style.display = "none";
		
		console.log("display", icon);
	}

	
	event.stopPropagation();

	actionPanelVisible(true);
}

function onActionSelect(el) {
	let action = el.getElementsByClassName("actionText")[0].textContent;
	
	actionPanelVisible(false);			

	requestDeviceState(SelectedDeviceID, action);
}

function hideMenuOnOutsideClick(event) {   
	let menu = document.getElementById("actionPanel");

	if (!menu.contains(event.target))
		actionPanelVisible(false);
}
			
function actionPanelVisible(visible) {
	let menu = document.getElementById("actionPanel");
	
	if (visible) {
		if (menu.hasAttribute("hidden")) {
			window.addEventListener('click', hideMenuOnOutsideClick);

			menu.removeAttribute("hidden");
		}
	}
	else 
		if (!menu.hasAttribute("hidden")) {
			menu.setAttribute("hidden", "");
		
			window.removeEventListener('click', hideMenuOnOutsideClick)
		}
}

window.addEventListener("click", function(event) {
		//console.log("window.addEventListener");
		//actionPanelVisible(false);
});


//$('.deviceName').dblclick(function(){
//	console.log('dbclick');
//    alert("The paragraph was double-clicked.");

//	this.setAttribute('disabled', 'true');
//});
		
function OnDeviceNameKeyPress(event) {
	let target = event.target;
	
	if (event.keyCode == 13 && target.value != target.name)
		RequestNewDeviceName(target.placeholder, target.value);

	if (event.keyCode == 13 || event.keyCode == 27)
		target.blur();

//    var regex = new RegExp('^[А-Ьа-ьA-Za-z0-9_ ]+$');
//    var key = String.fromCharCode(!event.charCode ? event.which : event.charCode);
//    if (!regex.test(key)) {
//		event.preventDefault();
//		return false;
//	}

    //alert(event.keyCode);
}

function OnDeviceNameEditStarted(event) {
	console.log("OnDeviceNameEditStarted");
	let field = event.target.querySelector(".deviceName");
	
	field.style="pointer-events:auto";
	field.readOnly = false;
	// field.select();
	field.focus();
}

function OnDeviceNameEditFinished(event) {
	console.log("OnDeviceNameEditFinished");

	let field = event.target;
	field.value = field.name;	

	field.readOnly = true;
	field.style="pointer-events:none";

	// field.selectionStart = 0;
	// field.selectionEnd = 0;
}

function RequestNewDeviceName(id, name) {
	let url = window.location.href + `request_name?id=${id}&name=${name}`;

	httpGet(url);
}

function UpdateScheme() {
	// document.getElementById("mainScheme").contentWindow.UpdateScheme();
	if (checkIframeLoaded())
		mainScheme.contentWindow.UpdateScheme();
}

function localTest() {
	let dv_info = { 'id' : "24c438bd9e7c0001", 'name' : "Lift 1", 'state': "Free", 'is_connected' : true };
	
	addDevice(dv_info);
	
	dv_info = { 'id' : "3451234145256345", 'name' : "Lift 2", 'state': "Busy", 'is_connected' : false };
	addDevice(dv_info);
	
	//		addDevice("1", "Free", "", false);
	//		addDevice("2", "Busy", "");
	//		addDevice("3", "SOS", "");
	//		setDeviceState("0", "Ready");
	//		removeDevice("1");
}		

function OnSchemeContainerLoaded(obj) {
    // obj.style.height = obj.contentWindow.document.documentElement.scrollHeight + 'px';

	UpdateScheme();
}

var pressTimer;

function OnDeviceNameContainerTouchStart(e) 
{
	console.log("OnDeviceNameContainerTouchStart", e.srcElement);

	e.preventDefault();

	pressTimer = window.setTimeout(function() { 
		OnDeviceNameEditStarted(e);
	}, 1000);
}

function OnDeviceNameContainerTouchEnd(e) 
{
	console.log("OnDeviceNameContainerTouchEnd", e.srcElement);

	e.preventDefault();

	clearTimeout(pressTimer);
}

function checkIframeLoaded() {
	return  (typeof mainScheme.contentWindow.UpdateScheme === "function");
    // var iframeDoc = mainScheme.contentDocument || mainScheme.contentWindow.document;

    // return iframeDoc.readyState  == 'complete';
} 

serverSendInit();
localTest();
