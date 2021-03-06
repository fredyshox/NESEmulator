//
//  GameViewController.h
//  nescocoa
//
//  Created by Kacper Raczy on 06/05/2019.
//  Copyright © 2019 Kacper Raczy. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <sys/time.h>
#import <nes/console.h>
#import <nes/controller.h>
#import <nes/cartridge.h>
#import "NESView.h"
#import "NESGame.h"
#import "NESKeyMap.h"

NS_ASSUME_NONNULL_BEGIN

@interface NESGameViewController : NSViewController {
    NSError* romLoadingError;
    NSThread* emulatorThread;
    nes_t* emulator;
    controller_state joypad1;
    controller_state joypad2;
}

@property (strong, readonly) NESView* nesView;
@property (strong, readonly) NESGame* game;
@property (strong, readonly) NESKeyMap* joypad1KeyMap;
@property (strong, readonly) NESKeyMap* joypad2KeyMap;
@property (atomic) BOOL isActive;

- (id)initWithGame:(NESGame*) game keyMap1: (NESKeyMap*) keyMap1 keyMap2: (NESKeyMap*) keyMap2;
- (void)loadGameFromRom;
- (void)emulateConsole;
- (void)startEmulation;
- (void)pauseEmulation;
- (double)getTimeUSec;

@end

NS_ASSUME_NONNULL_END
