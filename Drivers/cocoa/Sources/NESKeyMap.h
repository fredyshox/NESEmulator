//
//  NESKeyMap.h
//  nescocoa
//
//  Created by Kacper Raczy on 04/11/2019.
//  Copyright © 2019 Kacper Raczy. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <nes/controller.h>

#define NESKeyMapKeyCodeNone 0xffff

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSInteger, NESKeyMapSource) {
    NESKeyMapSourceKeyboard
};

@interface NESKeyMap : NSObject <NSSecureCoding>

@property NESKeyMapSource source;

- (id)initWithSource: (NESKeyMapSource) keyMapSource;
- (enum controller_button)buttonForKeyCode: (uint16_t) keyCode;
- (uint16_t) keyCodeForButton: (enum controller_button) button;
- (void)setKeyCode: (uint16_t) keyCode forButton: (enum controller_button) button;
- (void)clearKeyCodeForButton: (enum controller_button) button;
- (void)clearButtonForKeyCode: (uint16_t) keyCode;

@end

NS_ASSUME_NONNULL_END
