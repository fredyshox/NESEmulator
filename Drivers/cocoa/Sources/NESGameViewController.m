//
//  GameViewController.m
//  nescocoa
//
//  Created by Kacper Raczy on 06/05/2019.
//  Copyright © 2019 Kacper Raczy. All rights reserved.
//

#import "GameViewController.h"
#import <Carbon/Carbon.h>

@interface GameViewController ()
    @property (strong, readwrite, nonnull) NESView* nesView;
@end

@implementation GameViewController

- (void)loadView {
    [self setView: [[NSView alloc] init]];
}

- (instancetype)init {
    self = [super initWithNibName: nil bundle: nil];
    if (self) {
        emulator = malloc(sizeof(nes_t));
        nes_create(emulator);
    }
    return self;
}

- (void)dealloc {
    NSLog(@"dealloc");
    nes_free(emulator);
}

- (void)viewDidLoad {
    [super viewDidLoad];
    [[self view] setWantsLayer: YES];
    [[[self view] layer] setBackgroundColor: NSColor.blackColor.CGColor];
    
    NSLog(@"NES CPU freq: %f", NES_CPU_FREQ);
    _nesView = [[NESView alloc] initWithPPUHandle: emulator->ppu_handle];
    [_nesView setTranslatesAutoresizingMaskIntoConstraints: NO];
    [[self view] addSubview: _nesView];
    NSArray* hcon = [NSLayoutConstraint constraintsWithVisualFormat:@"H:|[nes]|" options: 0 metrics: nil views: @{@"nes": _nesView}];
    NSArray* vcon = [NSLayoutConstraint constraintsWithVisualFormat:@"V:|[nes]|" options:0 metrics: nil views: @{@"nes": _nesView}];
    [NSLayoutConstraint activateConstraints: hcon];
    [NSLayoutConstraint activateConstraints: vcon];
    
    NSString* gamePath = @"../../testsuite/roms/oam_stress/oam_stress.nes";
    [self loadGameFromFile: gamePath];
}

- (void)viewDidAppear {
    [super viewDidAppear];
    if (!nes_is_loaded(emulator)) {
        NSLog(@"Rom not loaded!");
        return;
    } else {
        [self startEmulation];
        [_nesView start];
    }
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

- (BOOL)acceptsFirstResponder {
    return YES;
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
