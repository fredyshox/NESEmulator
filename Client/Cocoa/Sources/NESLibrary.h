//
//  NESLibrary.h
//  nescocoa
//
//  Created by Kacper Raczy on 11/11/2019.
//  Copyright © 2019 Kacper Raczy. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "NESGame.h"

NS_ASSUME_NONNULL_BEGIN

@interface NESLibrary : NSObject

- (id)initWithPath:(NSString*) path;
- (NSArray*)loadGames;
- (BOOL)addGame:(NESGame*) game error: (NSError**) error;
- (BOOL)removeGame:(NESGame*) game error: (NSError**) error;
- (void)closeDatabase;

@end

NS_ASSUME_NONNULL_END
