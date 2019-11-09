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

NS_ASSUME_NONNULL_BEGIN

@interface NESGameViewController : NSViewController {
    NSThread* emulatorThread;
    nes_t* emulator;
    controller_state buttons;
}

@property (strong, readonly) NESView* nesView;
@property (strong, readonly) NESGame* game;
@property (atomic) BOOL isActive;

- (id)initWithGame:(NESGame*) game;
- (void)loadGameFromRom;
- (void)emulateConsole;
- (void) startEmulation;
- (void)pauseEmulation;
- (double)getTimeUSec;

@end

NS_ASSUME_NONNULL_END
