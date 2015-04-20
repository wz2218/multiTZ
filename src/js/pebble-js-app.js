//var mConfig = {};

Pebble.addEventListener("ready", 
	function(e) {
		console.log("multiTZ is ready - setn from .js");
  //loadLocalData();
  //returnConfigToPebble();
	}
);

/*
Pebble.addEventListener("showConfiguration", function(e) {
	Pebble.openURL(mConfig.configureUrl);
});

Pebble.addEventListener("webviewclosed",
  function(e) {
    if (e.response) {
      var config = JSON.parse(e.response);
      saveLocalData(config);
      returnConfigToPebble();
    }
  }
);

function saveLocalData(config) {

  //console.log("loadLocalData() " + JSON.stringify(config));

  localStorage.setItem("blink", parseInt(config.blink));  
  localStorage.setItem("invert", parseInt(config.invert)); 
  localStorage.setItem("bluetoothvibe", parseInt(config.bluetoothvibe)); 
  localStorage.setItem("hourlyvibe", parseInt(config.hourlyvibe)); 
  localStorage.setItem("branding_mask", parseInt(config.branding_mask)); 
  
  loadLocalData();

}
function loadLocalData() {
  
	mConfig.tzonename = parseInt(localStorage.getItem("tzonename"));
	mConfig.tzoneoffset = parseInt(localStorage.getItem("tzoneoffset"));
	mConfig.tztwoname = parseInt(localStorage.getItem("tztwoname"));
	mConfig.tztwooffset = parseInt(localStorage.getItem("tztwooffset"));
	mConfig.localoffset = parseInt(localStorage.getItem("localoffset"));
	mConfig.hourlyvibe = parseInt(localStorage.getItem("hourlyvibe"));
	mConfig.configureUrl = "http://www.googledrive.com/host/0BwhLBoN6lA6QcndzemFKRk42LTQ";

	if(isNaN(mConfig.tzonename)) {
		mConfig.tzonename = "CA1";
	}
	if(isNaN(mConfig.tzoneoffset)) {
		mConfig.invert = -8;
	}
	if(isNaN(mConfig.tztwoname)) {
		mConfig.tztwoname = "GER";
	}
	if(isNaN(mConfig.tztwooffset)) {
		mConfig.tztwooffset = 2;
	}
	if(isNaN(mConfig.localoffset)) {
		mConfig.localoffset = 8;
	}
	if(isNaN(mConfig.hourlyvibe)) {
		mConfig.hourlyvibe = 1;
	}   
	  


  //console.log("loadLocalData() " + JSON.stringify(mConfig));
}
function returnConfigToPebble() {
  //console.log("Configuration window returned: " + JSON.stringify(mConfig));
  Pebble.sendAppMessage({
    "blink":parseInt(mConfig.blink), 
    "invert":parseInt(mConfig.invert), 
    "bluetoothvibe":parseInt(mConfig.bluetoothvibe), 
    "hourlyvibe":parseInt(mConfig.hourlyvibe),
    "branding_mask":parseInt(mConfig.branding_mask)
  });    
}
*/

Pebble.addEventListener("showConfiguration",
  function(e) {
    Pebble.openURL("http://pebble-config.herokuapp.com/config?title=Timezone%20Settings&fields=Local%20Timezone%20UTC%20Offset,Timezone%201%20Name,Timezone%201%20UTC%20Offset,Timezone%202%20Name,Timezone%202%20UTC%20Offset"); 
	  // our dyanmic configuration page
  }
);
Pebble.addEventListener("webviewclosed",
  function(e) {
    var configuration = JSON.parse(e.response);
	  console.log(e.response);
    Pebble.sendAppMessage(configuration);
  }
);