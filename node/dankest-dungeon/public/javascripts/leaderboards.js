function setup() {
    var token = localStorage.getItem("dankest_token");
    var username = localStorage.getItem("dankest_user");

    $("#greeting").html(username);

    $.ajax({
        type: "POST",
        url: "api/CheckToken",
        data: {token: token},
        success: function (data) {
            if (data != "OK") {
                localStorage.removeItem("dankest_token");
                window.location = "index.html";
            } else {
                $.ajax({
                    type: "GET",
                    url: "/api/Leaderboard",
                    data: {token: localStorage.getItem("dankest_token")},
                    success: function (data) {
                        var body = "";

                        body += "<h2>Top Creators</h2>"
                        body += "<table border='1' padding='4' spacing='0'><tr style='font-weight:bold'><td>Adventurer</td><td>Levels Created</td></tr>";
                        for (var i=0; i<data.creators.length; ++i) {
                            body += "<tr><td>" + data.creators[i].username + "</td><td>" + data.creators[i].count + "</td></tr>";
                        }
                        body += "</table>";

                        body += "<h2>Top Levels</h2>"
                        body += "<table border='1' padding='4' spacing='0'><tr style='font-weight:bold'><td>Level</td><td>Rating</td></tr>";
                        for (var i=0; i<data.topDungeons.length; ++i) {
                            body += "<tr><td>" + data.topDungeons[i].name + "</td><td>" + data.topDungeons[i].rating + "</td></tr>";
                        }
                        body += "</table>";

                        body += "<h2>Most Frequent Players</h2>"
                        body += "<table border='1' padding='4' spacing='0'><tr style='font-weight:bold'><td>Adventurers</td><td>Plays</td></tr>";
                        for (var i=0; i<data.frequent.length; ++i) {
                            body += "<tr><td>" + data.frequent[i].username + "</td><td>" + data.frequent[i].count + "</td></tr>";
                        }
                        body += "</table>";


                        $("#boards").html(body);
                    },
                    dataType: "json"
                });
            }
        },
        dataType: "json"
    });
}
