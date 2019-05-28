//
//  GameViewController.m
//  nescocoa
//
//  Created by Kacper Raczy on 06/05/2019.
//  Copyright © 2019 Kacper Raczy. All rights reserved.
//

#import "GameViewController.h"

@interface GameViewController ()
    @property (strong, readwrite, nonnull) NESView* nesView;
@end

@implementation GameViewController

- (void)loadView {
    self.view = [[NSView alloc] init];
}

- (instancetype)init {
    self = [super initWithNibName: nil bundle: nil];
    if (self) {
        emulator = malloc(sizeof(nes_t));
        nes_create(emulator);
        lastTime = 0.0;
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
    
    _nesView = [[NESView alloc] initWithPPUHandle: emulator->ppu_handle];
    [_nesView setTranslatesAutoresizingMaskIntoConstraints: NO];
    [[self view] addSubview: _nesView];
    NSArray* hcon = [NSLayoutConstraint constraintsWithVisualFormat:@"H:|[nes]|" options: 0 metrics: nil views: @{@"nes": _nesView}];
    NSArray* vcon = [NSLayoutConstraint constraintsWithVisualFormat:@"V:|[nes]|" options:0 metrics: nil views: @{@"nes": _nesView}];
    [NSLayoutConstraint activateConstraints: hcon];
    [NSLayoutConstraint activateConstraints: vcon];
    
    NSString* gamePath = @"../../testsuite/Donkey Kong Classics (U).nes";
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
        while (![[NSThread currentThread] isCancelled]) {
            double current = [self getTimeUSec];
            if (lastTime == 0.0) {
                lastTime = current;
            }
            
            if (current != lastTime) {
                nes_step_time(emulator, current - lastTime);
            }
            
            lastTime = current;
            [NSThread sleepForTimeInterval: 0.005];
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
    
    if (nes_load_rom(emulator, c) != 0) {
        NSLog(@"Unable to load rom");
        return;
    }
}

@end
