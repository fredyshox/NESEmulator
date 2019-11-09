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
        memset(&buttons, 0, sizeof(controller_state));
    }
    
    return self;
}

- (void)dealloc {
    NSLog(@"gamevc dealloc");
    nes_free(emulator);
}

- (void)loadView {
    [self setView: [[NSView alloc] init]];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    [[self view] setWantsLayer: YES];
    [[[self view] layer] setBackgroundColor: NSColor.blackColor.CGColor];
    
    // reset button state
    controller_set_buttons(&emulator->controller1, buttons);
    controller_set_buttons(&emulator->controller2, buttons);
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
        int error = 0;
        double lastTime = [self getTimeUSec];
        
        while (![[NSThread currentThread] isCancelled] && error == 0) {
            [NSThread sleepForTimeInterval: 0.001];
            double current = [self getTimeUSec];
            
            if (current != lastTime) {
                nes_step_time(emulator, (current - lastTime) / 1E6, &error);
            }
            
            lastTime = current;
        }
        
        
        if (error != 0) {
            NSLog(@"Emulation stopped with error code: %d", error);
        }
    }
}

- (void)startEmulation {
    if (emulatorThread != nil) {
        [emulatorThread cancel];
        emulatorThread = nil;
    }
    
    NSThread* t = [[NSThread alloc] initWithTarget: self selector: @selector(emulateConsole) object: nil];
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
        return;
    }
    
    if (nes_load_rom(emulator, c) != 0) {
        NSLog(@"Unable to load rom");
        return;
    }
    // TODO cartridge memory
}

- (void)loadGameFromFile:(NSString *)file {
    cartridge* c = malloc(sizeof(cartridge));
    const char* _Nullable cFile = [file cStringUsingEncoding: NSUTF8StringEncoding];
    if (cFile == NULL) {
        NSLog(@"File path not UTF8");
        return;
    }
    
    if (cartridge_from_file(c, (char*) cFile) != 0) {
        NSLog(@"Unable to read cartridge from file %@", file);
        return;
    }
    
    NSLog(@"%d", c->mirroring_type);
    if (nes_load_rom(emulator, c) != 0) {
        NSLog(@"Unable to load rom");
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
    buttons.buttons[index] = false;
    controller_set_buttons(&emulator->controller1, buttons);
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
    buttons.buttons[index] = true;
    controller_set_buttons(&emulator->controller1, buttons);
}

@end
