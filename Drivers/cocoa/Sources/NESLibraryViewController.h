//
//  LibraryViewController.h
//  nescocoa
//
//  Created by Kacper Raczy on 08/11/2019.
//  Copyright © 2019 Kacper Raczy. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "NESGameCollectionViewItemDelegate.h"
#import "NESGameWindowController.h"
#import "NESGame.h"

NS_ASSUME_NONNULL_BEGIN

@interface NESLibraryViewController : NSViewController <NSCollectionViewDataSource, NSCollectionViewDelegate, NESGameCollectionViewItemDelegate>

@property (strong, readonly) NSCollectionView* collectionView;
@property (strong, readonly) NSArray<NESGame*>* games;
@property (strong, nullable) NESGameWindowController* gameWindowController;

- (IBAction)addRom:(id)sender;

@end

NS_ASSUME_NONNULL_END
