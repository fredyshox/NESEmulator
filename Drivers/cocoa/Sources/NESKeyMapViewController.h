//
//  NESSettingsViewController.h
//  nescocoa
//
//  Created by Kacper Raczy on 09/11/2019.
//  Copyright © 2019 Kacper Raczy. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <Carbon/Carbon.h>
#import "NESKeyMap.h"

NS_ASSUME_NONNULL_BEGIN

@interface NESKeyMapViewController : NSViewController <NSTableViewDelegate, NSTableViewDataSource>

@property (strong, readonly) NSString* userDefaultsKey;
@property (strong) NESKeyMap* keyMap;
@property (weak) IBOutlet NSTableView *tableView;
@property (weak) IBOutlet NSPopUpButton *inputButton;

- (id)initWithUserDefaultsKey:(NSString*) key;

@end

NS_ASSUME_NONNULL_END
