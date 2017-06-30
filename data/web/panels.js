
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
        $(document).on("change", "select[name=ntp]", function() { show_details("ntp_details", $(this), "on"); } );
        $(document).on("change", "select[name=mqtt]", function() { show_details("mqtt_details", $(this), "on"); } );

}
