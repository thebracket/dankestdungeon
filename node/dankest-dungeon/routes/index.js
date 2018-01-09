var express = require('express');
var router = express.Router();

/* GET home page. */
router.get('/', function(req, res, next) {
  res.render('index', { title: 'Express' });
});

module.exports = router;

router.post('/api/newUser', (req, res, next) => {
  let db = require("./database");
  let body = req.body;
  let tokens = require("./tokens");
  
  // Step 1 - Check uniqueness
  db.doSql("SELECT COUNT(*) AS count FROM adventurers WHERE username=$1;", [body.username], (countrows) => {
    let count = countrows[0].count;
    if (count != 0) {
      res.json("Error: username is taken");
    } else {
      // Step 2 - Do the insert
      db.doSql("INSERT INTO adventurers (username, password_hash) VALUES ($1, $2) RETURNING id;", [body.username, body.password], (raw) => {
        // Step 3 - Now we need to create a login token
        tokens.getNewToken({userId: raw[0].id}, (nt) => {
          res.json(nt);
        });
      });
    }
  });
});

router.post("/api/Login", (req, res, next) => {
  let db = require("./database");
  let body = req.body;
  let tokens = require("./tokens");

  // Check the username/password combo
  db.doSql("SELECT id FROM adventurers WHERE username=$1 AND password_hash=$2", [body.username, body.password], (rows) => {
    if (rows.length != 1) {
      res.json("Error: invalid username or password");
    } else {
      tokens.getNewToken({userId: rows[0].id}, (nt) => {
        res.json(nt);
      });
    }
  });
});

router.post("/api/CheckToken", (req, res, next) => {
  let db = require("./database");
  let body = req.body;
  let tokens = require("./tokens");

  tokens.expireTokens((r) => {
    db.doSql("SELECT * FROM tokens WHERE id=$1;", [body.token], (rows) => {
      if (rows.length != 1) {
        res.json("Invalid token");
      } else {
        res.json("OK");
      }
    });
  });
});

router.get("/api/StartGame/:token", (req, res, next) => {
  let db = require("./database");
  let body = req.body;
  let tokens = require("./tokens");
  const token = req.params.token;

  tokens.expireTokens((r) => {
    db.doSql("SELECT username FROM adventurers INNER JOIN tokens ON (adventurers.id = tokens.adventurer_id) WHERE tokens.id=$1", [token], (rows) => {
      if (rows.length != 1) {
        res.json("Error");
      } else {
        res.json(rows[0].username);
      }
    });
  });
});

router.post("/api/SaveDungeon", (req, res, next) => {
  let db = require("./database");
  let body = req.body;
  let tokens = require("./tokens");
  const token = body.token;

  tokens.expireTokens((r) => {
    db.doSql("SELECT adventurer_id FROM tokens WHERE id=$1", [token], (tokenrows) => {
      if (tokenrows.length != 1) {
        res.json("Error - who are you?");
      } else {
        var userId = tokenrows[0].adventurer_id;
        db.doSql("INSERT INTO dungeons (name, level_data, adventurer_id) VALUES ($1, $2, $3);", 
          [body.name, body.data, userId], (tmp) => {
            res.json("Saved");
          });
      }
    });
  });
});

router.get("/api/DungeonList", (req, res, next) => {
  let db = require("./database");
  db.doSql("SELECT dungeons.id AS id, name, username FROM dungeons INNER JOIN adventurers a ON dungeons.adventurer_id = a.id;", [], (rows) => {
    let result = "";
    for (var i=0; i<rows.length; ++i) {
      result += rows[i].id + "\n";
      result += rows[i].name + "\n";
      result += rows[i].username + "\n";
    }
    res.json(result);
  });
});
