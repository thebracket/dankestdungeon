module.exports = {
    expireTokens : function(callback) {
        let db = require("./database");
        const sql = "DELETE FROM tokens WHERE pointintime < (NOW() - interval '2 days')";
        db.doSql(sql, [], () => {
            callback();
        });
    },

    doesTokenExist : function(token) {
        let db = require("./database");
        const sql = "SELECT COUNT(*) FROM tokens WHERE id=$1";
        db.doSql(sql, [token], (rows) => {
            if (rows[0].count > 0) return true;
            return false;
        });
    },

    getNewToken : function(userInfo, callback) {
        let ok = false;
        let candidate = (Math.random() * 65536).toFixed(0);
        while (!ok) {
            ok = !this.doesTokenExist(candidate);
            if (!ok) candidate = (Math.random() * 65536).toFixed(0);
        }
        userInfo.token = candidate;

        let db = require("./database");
        const sql = "INSERT INTO tokens (id, adventurer_id) VALUES ($1, $2);";
        db.doSql(sql, [userInfo.token, userInfo.userId], () => {
            callback(userInfo);
        });
    }
}