#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>

// Function to execute SQL statements and handle errors
void executeSQL(sqlite3 *db, const char *sql) {
    char *errMsg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
    } else {
        printf("Operation executed successfully.\n");
    }
}

// Function to create the table
void createTable(sqlite3 *db) {
    const char *sql = "CREATE TABLE IF NOT EXISTS people ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "name TEXT NOT NULL, "
                      "age INTEGER NOT NULL);";
    executeSQL(db, sql);
}

// Function to insert data into the table
void insertData(sqlite3 *db) {
    const char *sql = "INSERT INTO people (name, age) VALUES ('Alice', 30);"
                      "INSERT INTO people (name, age) VALUES ('Bob', 25);"
                      "INSERT INTO people (name, age) VALUES ('Charlie', 35);";
    executeSQL(db, sql);
}

// Function to update a name in the table
void updateName(sqlite3 *db, const char *oldName, const char *newName) {
    char sql[256];
    snprintf(sql, sizeof(sql), "UPDATE people SET name = '%s' WHERE name = '%s';", newName, oldName);
    executeSQL(db, sql);
}

// Function to display the data
void displayData(sqlite3 *db) {
    const char *sql = "SELECT * FROM people;";
    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to retrieve data: %s\n", sqlite3_errmsg(db));
        return;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char *name = sqlite3_column_text(stmt, 1);
        int age = sqlite3_column_int(stmt, 2);
        printf("ID: %d, Name: %s, Age: %d\n", id, (const char*)name, age);  // Cast to const char*
    }

    sqlite3_finalize(stmt);
}

int main() {
    sqlite3 *db;
    int rc = sqlite3_open("example.db", &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    createTable(db);
    insertData(db);

    printf("Database content before update:\n");
    displayData(db);

    char oldName[100], newName[100];
    printf("Enter the name you want to change: ");
    scanf("%99s", oldName);
    printf("Enter the new name: ");
    scanf("%99s", newName);

    updateName(db, oldName, newName);

    printf("Database content after update:\n");
    displayData(db);

    sqlite3_close(db);
    return 0;
}
