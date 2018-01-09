module.exports = {
    connectionString : process.env.DATABASE_URL || 'postgres://postgres:postgres@172.16.10.193:5432/dankestdungeon',

    doSql : function(sql, params, callback) {
        console.log(sql);
        let pg = require('pg');
        let path = require('path');

        const results = [];
        pg.connect(this.connectionString, (err, client, done) => {
            // Handle connection errors
            if (err) {
                done();
                console.log(err);
                return res.status(500).json({success: false, data: err});
            }
            // SQL Query > Select Data
            const query = client.query(sql, params);
        // Stream results back one row at a time
        query.on('row', (row) => {
            results.push(row);
        });
        // After all data is returned, close connection and return results
        query.on('end', () => {
            done();
            callback(results);
            });
        });
    }
}
