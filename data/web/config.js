//
// check if alive
//
function alive() {
        $.ajax({
                type: "POST",
                url: "./api.html",
                data: "input={ vvvv }",
                success: function() {
                          return true;
                }
        });
        return false;
}

function reloadIfAlive() {

  if(alive()) {
    window.location="/";
  } else {
   setTimeout( reloadIfAlive(), 5000);
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
              console.log("Setting "+this.name+" to "+json[this.name]);
              return json[this.name];
            });
            // set radio groups
            set_radio("wifi", json["wifi"]);
            set_radio("lan", json["lan"]);
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
