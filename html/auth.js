//
// login
//
function login() {
		username=$("#username").val();
		password=$("#password").val();
		$.ajax({
			type: "POST",
			url: "./auth.html",
			data: "action=login&username="+username+"&password="+password,
			success: function(html) {
				if(html=='true')    {
					window.location="/";
				}
				else    {
					$("#failed").popup("open");
				}
			}
		});
		return false;
	};

//
// logout
//
function logout() {
        $.ajax({
                type: "POST",
                url: "./auth.html",
                data: "action=logout",
                success: function(html) {
                          window.location="/";
                }
        });
        return false;
};
