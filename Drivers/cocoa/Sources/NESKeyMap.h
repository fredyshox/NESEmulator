//
//  NESKeyMap.h
//  nescocoa
//
//  Created by Kacper Raczy on 04/11/2019.
//  Copyright © 2019 Kacper Raczy. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <nes/controller.h>

NS_ASSUME_NONNULL_BEGIN

@interface NESKeyMap : NSObject <NSCoding>

- (id)init;
- (enum controller_button)buttonForKeyCode: (uint16_t) keyCode;
- (void)setKeyCode: (uint16_t) keyCode forButton: (enum controller_button) button;

@end

NS_ASSUME_NONNULL_END
