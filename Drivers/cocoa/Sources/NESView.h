//
//  NESView.h
//  nescocoa
//
//  Created by Kacper Raczy on 13/04/2019.
//  Copyright © 2019 Kacper Raczy. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <stdint.h>
#import <ppu/renderer.h>
#import <nes/console.h>

NS_ASSUME_NONNULL_BEGIN

@interface NESView : NSView

@property unsigned int fps;
@property (nullable) volatile ppu_render_handle* renderingHandle;

- (instancetype)initWithPPUHandle:(nullable ppu_render_handle*) handle;
- (void)renderFrame:(id) sender;
- (void)startRendering;
- (void)pauseRendering;

@end

NS_ASSUME_NONNULL_END
