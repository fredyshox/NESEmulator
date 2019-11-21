//
//  GameViewController.m
//  nescocoa
//
//  Created by Kacper Raczy on 06/05/2019.
//  Copyright © 2019 Kacper Raczy. All rights reserved.
//

#import "NESGameViewController.h"
#import <Carbon/Carbon.h>

@implementation NESGameViewController

@synthesize nesView = _nesView;
@synthesize game = _game;

- (id)initWithGame:(NESGame*) game {
    self = [super initWithNibName: nil bundle: nil];
    if (self) {
        _game = game;
        emulator = malloc(sizeof(nes_t));
        nes_create(emulator);
        memset(&joypad, 0, sizeof(controller_state));
    }
    
    return self;
}

- (void)dealloc {
    NSLog(@"gamevc dealloc");
    if (nes_is_loaded(emulator)) {
        mapper_free(emulator->mapper);
        free(emulator->mapper);
        cartridge_free(emulator->cartridge);
        free(emulator->cartridge);
    }
    nes_free(emulator);
}

- (void)loadView {
    [self setView: [[NSView alloc] init]];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    [[self view] setWantsLayer: YES];
    [[[self view] layer] setBackgroundColor: NSColor.blackColor.CGColor];
    if (_game == nil) {
        [NSException raise: @"GameNotSet" format: @"NESGame is set to nil"];
    }
    
    // reset button state
    controller_set_buttons(&emulator->controller1, joypad);
    controller_set_buttons(&emulator->controller2, joypad);
    // init views
    NSLog(@"NES CPU freq: %f", NES_CPU_FREQ);
    _nesView = [[NESView alloc] initWithPPUHandle: emulator->ppu_handle];
    [[self view] addSubview: _nesView];
    
    // load file
    [self loadGameFromRom];
}

- (void)viewDidLayout {
    [super viewDidLayout];
    [_nesView setFrame: [self.view frame]];
}

- (void)viewDidAppear {
    [super viewDidAppear];
    if (!nes_is_loaded(emulator)) {
        NSLog(@"Rom not loaded!");
        if (romLoadingError != nil) {
            NSAlert* alert = [NSAlert alertWithError: romLoadingError];
            [alert runModal];
        }
        return;
    } else {
        [self startEmulation];
        [_nesView startRendering];
    }
}

- (void)viewWillDisappear {
    [super viewWillDisappear];
    [self pauseEmulation];
    [_nesView pauseRendering];
}

- (BOOL)acceptsFirstResponder {
    return YES;
}

- (double)getTimeUSec {
    struct timeval time;
    gettimeofday(&time, NULL);
    
    return ((double) time.tv_sec * 1E6 + (double) time.tv_usec);
}

- (void)emulateConsole {
    @autoreleasepool {
        int errorCode = 0;
        double lastTime = [self getTimeUSec];
        
        while (![[NSThread currentThread] isCancelled] && errorCode == 0) {
            [NSThread sleepForTimeInterval: 0.001];
            double current = [self getTimeUSec];
            
            if (current != lastTime) {
                nes_step_time(emulator, (current - lastTime) / 1E6, &errorCode);
            }
            
            lastTime = current;
        }
        
        
        if (errorCode != 0) {
            NSString* domain = [[NSBundle mainBundle] bundleIdentifier];
            NSString* message = [NSString stringWithFormat: @"Emulation stopped with error code: %d", errorCode];
            NSDictionary* userInfo = @{
                NSLocalizedDescriptionKey: message
            };
            
            NSError* emuError = [NSError errorWithDomain: domain
                                                    code: 0x3000
                                                userInfo: userInfo];
            dispatch_async(dispatch_get_main_queue(), ^{
                [[NSAlert alertWithError: emuError] runModal];
            });
        }
    }
}

- (void)startEmulation {
    if (emulatorThread != nil) {
        [emulatorThread cancel];
        emulatorThread = nil;
    }
    
    NSThread* t = [[NSThread alloc] initWithTarget: self selector: @selector(emulateConsole) object: nil];
    [t setQualityOfService: NSQualityOfServiceUserInteractive];
    [t setThreadPriority: 1.0];
    [t start];
    emulatorThread = t;
}

- (void)pauseEmulation {
    if (emulatorThread != nil) {
        [emulatorThread cancel];
    } else {
        NSLog(@"Emulation is not even started!");
    }
}

- (void)loadGameFromRom {
    NSError* error = nil;
    cartridge* c = [_game loadRomWithError: &error];
    if (error != nil) {
        NSLog(@"Error: %@", [error localizedDescription]);
        romLoadingError = error;
        return;
    }
    
    int romStatus = nes_load_rom(emulator, c);
    if (romStatus != 0) {
        NSString* domain = [[NSBundle mainBundle] bundleIdentifier];
        NSString* message = [NSString stringWithFormat: @"Mapper %d is not supported.", c->mapper];
        NSDictionary* userInfo = @{
            NSLocalizedDescriptionKey: message
        };
        
        romLoadingError = [NSError errorWithDomain: domain
                                             code: 0x2000
                                         userInfo: userInfo];
        NSLog(@"Unable to load rom");
        // free memory from unused cartridge
        cartridge_free(c);
        free(c);
        return;
    }
}

- (void)keyUp:(NSEvent *)event {
    int index;
    switch ([event keyCode]) {
        case kVK_ANSI_A:
            index = A_BUTTON;
            break;
        case kVK_ANSI_S:
            index = B_BUTTON;
            break;
        case kVK_UpArrow:
            index = UP_BUTTON;
            break;
        case kVK_DownArrow:
            index = DOWN_BUTTON;
            break;
        case kVK_LeftArrow:
            index = LEFT_BUTTON;
            break;
        case kVK_RightArrow:
            index = RIGHT_BUTTON;
            break;
        case kVK_Escape:
            index = SELECT_BUTTON;
            break;
        case kVK_Return:
            index = START_BUTTON;
            break;
        default: return;
    }
    joypad.buttons[index] = false;
    controller_set_buttons(&emulator->controller1, joypad);
}

- (void)keyDown:(NSEvent *)event {
    NSLog(@"Key down %d, %@", [event keyCode], [event characters]);
    int index;
    switch ([event keyCode]) {
        case kVK_ANSI_A:
            index = A_BUTTON;
            break;
        case kVK_ANSI_S:
            index = B_BUTTON;
            break;
        case kVK_UpArrow:
            index = UP_BUTTON;
            break;
        case kVK_DownArrow:
            index = DOWN_BUTTON;
            break;
        case kVK_LeftArrow:
            index = LEFT_BUTTON;
            break;
        case kVK_RightArrow:
            index = RIGHT_BUTTON;
            break;
        case kVK_Escape:
            index = SELECT_BUTTON;
            break;
        case kVK_Return:
            index = START_BUTTON;
            break;
        default: return;
    }
    joypad.buttons[index] = true;
    controller_set_buttons(&emulator->controller1, joypad);
}

@end
