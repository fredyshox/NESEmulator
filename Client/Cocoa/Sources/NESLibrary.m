//
//  NESLibrary.m
//  nescocoa
//
//  Created by Kacper Raczy on 11/11/2019.
//  Copyright © 2019 Kacper Raczy. All rights reserved.
//

#import "NESLibrary.h"
#import <sqlite3.h>

@implementation NESLibrary {
    BOOL isOpen;
    NSString* databasePath;
    sqlite3* database;
}

- (id)initWithPath:(NSString*) path {
    if (self = [super init]) {
        databasePath = path;
        isOpen = YES;
        const char* cPath = [path UTF8String];
        int res = sqlite3_open(cPath, &database);
        if (res != SQLITE_OK) {
            NSLog(@"Error opening database");
            return nil;
        }
        [self initializeDatabase];
    }
    
    return self;
}

- (void)initializeDatabase {
    NSString* query =
        @"CREATE TABLE IF NOT EXISTS game_library ( "
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "title TEXT NOT NULL, "
        "path TEXT NOT NULL "
        ");";
    char* errorMsg = NULL;
    if (sqlite3_exec(database, [query UTF8String], NULL, NULL, &errorMsg) != SQLITE_OK) {
        NSLog(@"Error while creating db table: %s", errorMsg);
        sqlite3_free(errorMsg);
    } else {
        NSLog(@"Db table created successfully!");
    }
}

- (void)dealloc {
    NSLog(@"Closing sqlite database");
    sqlite3_close(database);
}

- (NSArray*)loadGames {    
    NSString* query =
        @"SELECT id, title, path FROM game_library "
        "ORDER BY id ASC;";
    NSMutableArray<NESGame*>* games = [NSMutableArray array];
    sqlite3_stmt* statement;
    if (sqlite3_prepare_v2(database, [query UTF8String], -1, &statement, NULL) == SQLITE_OK) {
        while (sqlite3_step(statement) == SQLITE_ROW) {
            int identifier = sqlite3_column_int(statement, 0);
            const char* cTitle = (const char*) sqlite3_column_text(statement, 1);
            const char* cPath =  (const char*) sqlite3_column_text(statement, 2);
            NSString* title = [NSString stringWithUTF8String: cTitle];
            NSURL* path = [NSURL URLWithString: [NSString stringWithUTF8String: cPath]];
            if (title == nil || path == nil)
                continue;
            [games addObject: [[NESGame alloc] initWithId: identifier andTitle: title andPath: path]];
        }
        sqlite3_finalize(statement);
    } else {
        NSLog(@"Error while loading games: %s", sqlite3_errmsg(database));
    }
    
    return games;
}

- (NSError*) databaseError {
    NSString* domain = [[NSBundle mainBundle] bundleIdentifier];
    NSDictionary *userInfo = @{
        NSLocalizedDescriptionKey: [NSString stringWithUTF8String: sqlite3_errmsg(database)]
    };
    NSError* error = [[NSError alloc] initWithDomain: domain code: 1 | 0x2000 userInfo: userInfo];
    return error;
}

- (BOOL)addGame:(NESGame*) game error: (NSError**) error {
    NSString* query =
        @"INSERT INTO game_library (title, path) "
        "VALUES (?, ?);";
    sqlite3_stmt* statement;
    *error = nil;
    if (sqlite3_prepare_v2(database, [query UTF8String], -1, &statement, NULL) == SQLITE_OK) {
        sqlite3_bind_text(statement, 1, [[game title] UTF8String], -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(statement, 2, [[[game path] absoluteString] UTF8String], -1, SQLITE_TRANSIENT);
        if (sqlite3_step(statement) != SQLITE_DONE) {
            NSLog(@"SQL Insert error");
            *error = [self databaseError];
        }
        
        sqlite3_finalize(statement);
    } else {
        NSLog(@"Error while adding game");
        *error = [self databaseError];
    }
    
    return error != nil;
}

- (BOOL)removeGame:(NESGame*) game error: (NSError**) error {
    NSString* query =
        @"DELETE FROM game_library WHERE id = ?;";
    sqlite3_stmt* statement;
    *error = nil;
    if (sqlite3_prepare_v2(database, [query UTF8String], -1, &statement, NULL) == SQLITE_OK) {
        sqlite3_bind_int(statement, 1, [game identifier]);
        if (sqlite3_step(statement) != SQLITE_DONE) {
            NSLog(@"SQL Delete error.");
            *error = [self databaseError];
        }
        
        sqlite3_finalize(statement);
    } else {
        NSLog(@"Error while removing game");
        *error = [self databaseError];
    }
    
    return error != nil;
}

- (void)closeDatabase {
    if (isOpen) {
        NSLog(@"Closing sqlite database");
        sqlite3_close(database);
        isOpen = NO;
    }
}

@end
