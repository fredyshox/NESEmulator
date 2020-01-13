//
//  NESKeyMapViewControllerDelegate.h
//  nescocoa
//
//  Created by Kacper Raczy on 14/11/2019.
//  Copyright © 2019 Kacper Raczy. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "NESKeyMap.h"

@protocol NESKeyMapViewControllerDelegate
- (void) keyCodeDidBecomeAssigned: (uint16_t) keyCode forUserDefaultsKey: (NSString*) key;
@end
