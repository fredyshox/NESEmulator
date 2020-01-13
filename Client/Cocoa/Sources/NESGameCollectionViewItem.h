//
//  GameCollectionViewItem.h
//  nescocoa
//
//  Created by Kacper Raczy on 08/11/2019.
//  Copyright © 2019 Kacper Raczy. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "NESGameCollectionViewItemDelegate.h"

NS_ASSUME_NONNULL_BEGIN

@interface NESGameCollectionViewItem : NSCollectionViewItem

@property (weak, nullable) id<NESGameCollectionViewItemDelegate> delegate;

- (IBAction)playGame:(NSMenuItem *)sender;
- (IBAction)showInFinder:(NSMenuItem *)sender;
- (IBAction)deleteGame:(NSMenuItem *)sender;

@end

NS_ASSUME_NONNULL_END
