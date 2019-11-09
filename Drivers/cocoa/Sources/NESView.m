//
//  NESView.m
//  nescocoa
//
//  Created by Kacper Raczy on 13/04/2019.
//  Copyright © 2019 Kacper Raczy. All rights reserved.
//

#import "NESView.h"

@implementation NESView {
    // private fields
    NSTimer *frameTimer;
    CGContextRef bitmapContext;
    uint8_t *bitmapBuffer;
    int frameCounter;
}

@synthesize fps, renderingHandle;

- (instancetype)init {
    self = [self initWithPPUHandle: NULL];
    return self;
}

- (instancetype)initWithPPUHandle: (nullable ppu_render_handle*) handle {
    self = [super initWithFrame: NSZeroRect];
    if (self) {
        [self initializeWithHandle: handle];
    }
    return self;
}

- (instancetype)initWithCoder:(NSCoder *)decoder {
    self = [super initWithCoder: decoder];
    if (self) {
        [self initializeWithHandle: NULL];
    }
    return self;
}

- (instancetype)initWithFrame:(NSRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        [self initializeWithHandle: NULL];
    }
    return self;
}

-(void) initializeWithHandle: (ppu_render_handle*) handle {
    self->bitmapBuffer = [self createBuffer];
    self->renderingHandle = handle;
    self->fps = 60;
    CGColorSpaceRef cs = CGColorSpaceCreateDeviceRGB();
    self->bitmapContext = CGBitmapContextCreate(bitmapBuffer, HORIZONTAL_RES, VERTICAL_RES, 8, HORIZONTAL_RES * 4, cs, kCGImageAlphaNoneSkipFirst);
}

- (uint8_t*) createBuffer {
    return malloc(sizeof(uint8_t) * 4 * HORIZONTAL_RES * VERTICAL_RES);
}

- (void)dealloc {
    NSLog(@"nesview dealloc");
    [frameTimer invalidate];
    frameTimer = nil;
    free(bitmapBuffer);
}

- (void)drawRect:(NSRect)dirtyRect {
    if (renderingHandle == NULL || bitmapContext == NULL) {
        NSLog(@"Rendering handle not present!");
        return;
    }
    
    // copy frame data
//    for (int i = 0; i < VERTICAL_RES * 3; i++) {
//        for (int j = 0; j < HORIZONTAL_RES; j++) {
//            for (int k = 0; k < 3; k++) {
//                bitmapBuffer[i * HORIZONTAL_RES + j * 4 + k + 1] = rand; //renderingHandle->frame[i * HORIZONTAL_RES + j * 3 + k];
//            }
//        }
//    }
    
    for (int i = 0; i < VERTICAL_RES; i++) {
        for (int j = 0, k = 0; j < HORIZONTAL_RES * 4; j++) {
            if (j % 4 != 0) {
                bitmapBuffer[i * HORIZONTAL_RES * 4 + j] = renderingHandle->frame[i * HORIZONTAL_RES * 3 + k];
                k += 1;
            }
        }
    }
    
    frameCounter += 1;
    if (frameCounter % 60 == 0) {
        NSLog(@"C: %d L: %d FB: %d", renderingHandle->cycle, renderingHandle->line, renderingHandle->frame_buf_pos);
    }
    
    if ([NSGraphicsContext currentContext] != nil) {
        CGContextRef currentContext = [[NSGraphicsContext currentContext] CGContext];
        CGImageRef frame = CGBitmapContextCreateImage(bitmapContext);
        CGRect rect = NSRectToCGRect(self.bounds);
        CGContextDrawImage(currentContext, rect, frame);
        CGImageRelease(frame);
    }
}

- (void)renderFrame:(NSTimer *)sender {
    [self setNeedsDisplay: YES];
}

- (void)startRendering {
    if (renderingHandle == NULL) {
        NSLog(@"Cannot start rendering without ppu handle!");
        return;
    }
    
    if (frameTimer != nil) {
        [frameTimer invalidate];
        frameTimer = nil;
    }
    
    frameTimer = [NSTimer scheduledTimerWithTimeInterval: (1.0/fps) target: self selector: @selector(renderFrame:) userInfo: nil repeats: YES];
    [[NSRunLoop currentRunLoop] addTimer: frameTimer forMode: NSDefaultRunLoopMode];
}

- (void)pauseRendering {
    [frameTimer invalidate];
    frameTimer = nil;
}

@end
