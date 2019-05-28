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
#import <nes/cartridge.h>
#import "NESView.h"

NS_ASSUME_NONNULL_BEGIN

@interface GameViewController : NSViewController {
    NSThread* emulatorThread;
    nes_t* emulator;
    double lastTime;
}

@property (strong, readonly, nonnull) NESView* nesView;
@property (atomic) BOOL isActive;

- (void)loadGameFromFile:(NSString*) file;
- (void)emulateConsole;
- (void) startEmulation;
- (void)pauseEmulation;
- (double)getTimeUSec;

@end

NS_ASSUME_NONNULL_END
