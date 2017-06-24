//
// apply
//

$(document).ready( function() {
  config();
}
);


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
