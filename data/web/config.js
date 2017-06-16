//
// apply
//

$(document).ready( function() {
}
);

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
          success: function() { 
            setTimeout( function() { window.location="/"; }, 20000); 
          }
    }
  );
  
}

//
// get configuration as JSON and populate inputs
//
function config() {
 
  // form
  var configForm= $('#configForm');

  $.ajax(
    {     type: "POST",
          url: "/",
          data: "action=config",
          success: function(json) { 
            console.log(json);
            configForm.find('input').val( function() {
              return json[this.name];
            })
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
