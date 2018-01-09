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
            }
        },
        dataType: "json"
    });
}

function saveLevel(levelName, levelData) {
    $.ajax({
        type: "POST",
        url: "api/SaveDungeon",
        data: {name: levelName, data: levelData, token: localStorage.getItem("dankest_token")},
        success: function (data) {
            console.log(data);
        },
        dataType: "json"
    });
}