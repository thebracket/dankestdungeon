function setup() {
    var loginButton = document.getElementById('loginBtn');
    var newUserButton = document.getElementById('newUserBtn');

    var usernameField = document.getElementById('username');

    // Do we have a cached user?
    var cachedUsername = localStorage.getItem("dankest_user");
    usernameField.value = cachedUsername;

    // Set focus
    usernameField.focus();

    loginButton.onclick = function(e) {
        var user = {
            username : document.getElementById("username").value,
            password: md5(document.getElementById("password").value)
        };
        $.ajax({
            type: "POST",
            url: "api/Login",
            data: user,
            success: function (data) {
                if (data == "Error: invalid username or password") {
                    alert("Error: invalid username or password");
                } else {
                    localStorage.setItem("dankest_user", user.username);
                    localStorage.setItem("dankest_token", data.token);
                    window.location = "game.html";
                }
            },
            dataType: "json"
        });
    };
    newUserButton.onclick = function(e) {
        var newUser = {
            username : document.getElementById("nusername").value,
            password: document.getElementById("npassword").value,
            password2: document.getElementById("npassword2").value
        };
        
        var valid = true;
        var errors = "";
        if (newUser.username.length < 4) {
            valid = false;
            errors += "Please select a username of at least 4 characters in length."
        }
        if (newUser.password.length < 4) {
            valid = false;
            errors += "Please select a password of 7 characters or more."
        }
        if (newUser.password !== newUser.password2) {
            valid = false;
            errors += "Your new passwords don't match. You really do type as badly as me, sorry!";
        }

        if (!valid) {
            alert(errors);
            return;
        } else {
            localStorage.setItem("dankest_user", newUser.username);
            delete newUser.password2;
            newUser.password = md5(newUser.password);

            $.ajax({
                type: "POST",
                url: "/api/newUser",
                data: newUser,
                success: function (data) {
                    console.log("Returned data", data);
                    if (data == "Error: username is taken") {
                        alert("Sorry - that username is taken. Try another one.");
                    } else {
                        localStorage.setItem("dankest_token", data.token);
                        window.location = "game.html";
                    }
                },
                dataType: "json"
            });
        }
    };
}