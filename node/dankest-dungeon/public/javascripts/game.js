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
    if (levelName.length < 1) levelName = "Nameless";
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

function getRating() {
    var rating = -1;
    while (rating <0 || rating > 5) {
        var temp = prompt("Please rate this level from 0 to 5 (5 being best)", "3");
        try {
            rating = parseInt(temp);
        } catch (e) {
            rating = -1;
        }
    }
    return rating;
}

function winGame(turns, levelId) {
    var token = localStorage.getItem("dankest_token");
    alert("Congratulations! You have beaten the dungeon, in " + turns + " turns.");
    var rating = getRating();
    $.ajax({
        type: "POST",
        url: "api/SaveRunthrough",
        data: {token: token, levelId: levelId, turns: turns, rating:rating, result: "WIN"},
        success: function (data) {
            console.log(data);
        },
        dataType: "json"
    });
}

function loseGame(turns, levelId) {
    var token = localStorage.getItem("dankest_token");
    alert("You are dead! You lasted " + turns + " turns. Hopefully you can do better next time.");
    var rating = getRating();
    $.ajax({
        type: "POST",
        url: "api/SaveRunthrough",
        data: {token: token, levelId: levelId, turns: turns, rating:rating, result: "LOSE"},
        success: function (data) {
            console.log(data);
        },
        dataType: "json"
    });
}