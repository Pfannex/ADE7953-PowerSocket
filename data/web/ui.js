// ---------------------------
// initialization
// ---------------------------

$(document).ready( function() {

        console.log('Initializing ui...');
        set_radio_handlers();
        //set_select_handlers();
        set_config();
        consStart();
        }
);

// ---------------------------
// panels
// ---------------------------

// show panel panelName if radio has value
function show_details(panelName, radio, value) {

        var panel= $("#"+panelName);
        var v= radio.val();
        console.log("panel "+panelName+", radio is "+v+", show if "+value);
        if(v.match(value)) {
                panel.show();
        } else {
                panel.hide();
        }

}

function set_radio(name, value) {

        console.log("Setting radio "+name+" to "+value);
        var radio= $('input:radio[name="'+name+'"]');
        button= radio.filter('[value="'+value+'"]');
        button.prop('checked', true);
        radio.checkboxradio('refresh');
        button.trigger('change');
}

function set_radio_handlers() {

        console.log("Setting radio handlers...");
        $(document).on("change", "input[name=wifi]", function()
          { show_details("wifi_net_details", $(this), "manual");
            show_details("wifi_details", $(this), "dhcp|manual");
          } );
        $(document).on("change", "input[name=lan]", function() { show_details("lan_net_details", $(this), "manual"); } );
        $(document).on("change", "input[name=ntp]", function() { show_details("ntp_details", $(this), "on"); } );
        $(document).on("change", "input[name=mqtt]", function() { show_details("mqtt_details", $(this), "on"); } );

}

// ---------------------------
// config
// ---------------------------

//
// check if alive
//
function alive() {
        var result= false;
        $.ajax({
                type: "POST",
                url: "./api.html",
                data: "",
                timeout: 1000,
                success: function() {
                          console.log("server is alive");
                          result= true;
                },
                error:  function() {
                          console.log("server is dead");
                          result= false;
                }
        });
        return result;
}

function reloadIfAlive() {

  console.log("reload if alive...");
  if(alive()) {
    window.location="/";
  } else {
   setTimeout( "reloadIfAlive()", 5000);
  }

}

//
// post action reboot
//
function reboot() {

  // show popup
  $("#popupRebooting").popup("open");

  $.ajax(
    {     type: "POST",
          url: "/",
          data: "action=reboot",
          success: function() { reloadIfAlive;  }
    }
  );

}

//
// get configuration as JSON and populate inputs
//
function set_config() {

  console.log("Setting configuration...")
  // form
  var configForm= $('#configForm');

  $.ajax(
    {     type: "POST",
          url: "/",
          data: "action=config",
          success: function(json) {
            // log json
            console.log(json);
            // set inputs
            configForm.find('input:text').val( function() {
              console.log("Setting text input "+this.name+" to "+json[this.name]);
              return json[this.name];
            });
            // set passwords
            configForm.find('input:password').val( function() {
              console.log("Setting password input "+this.name+" to "+json[this.name]);
              return json[this.name];
            });
            // set radio groups
            set_radio("wifi", json["wifi"]);
            set_radio("lan", json["lan"]);
            set_radio("ntp", json["ntp"]);
            set_radio("update", json["update"]);
            set_radio("mqtt", json["mqtt"]);
          }
    }
  );

}

//
// post all named fields in the form
//
function apply() {

  // form
  var configForm= $('#configForm');

  // show popup
  $("#popupProcessing").popup("open");

  //console.log(configForm.serialize());

  // submit the form
  $.post("/", configForm.serialize(), function(data) {
    // hide popup
    $("#popupProcessing").popup("close");
  });



}
