//
//  NSUserDefaults+NSUserDefaults_NESKeyMap.h
//  nescocoa
//
//  Created by Kacper Raczy on 09/12/2019.
//  Copyright © 2019 Kacper Raczy. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "NESKeyMap.h"

NS_ASSUME_NONNULL_BEGIN

@interface NSUserDefaults (NESKeyMapSupport)
- (NESKeyMap* _Nullable)loadKeyMapUsingKey: (const NSString* _Nonnull) key;
- (void)storeKeyMap: (NESKeyMap* _Nonnull) keyMap usingKey: (const NSString* _Nonnull) key;
@end

NS_ASSUME_NONNULL_END
