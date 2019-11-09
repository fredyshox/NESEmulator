//
//  GameCollectionViewItemDelegate.h
//  nescocoa
//
//  Created by Kacper Raczy on 09/11/2019.
//  Copyright © 2019 Kacper Raczy. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class NESGameCollectionViewItem;

NS_ASSUME_NONNULL_BEGIN

@protocol NESGameCollectionViewItemDelegate <NSObject>
@optional
- (void)gcvItemDidReceivePlayGameRequest:(NESGameCollectionViewItem *)item;
- (void)gcvItemDidReceiveDeleteGameRequest:(NESGameCollectionViewItem *)item;
@end

NS_ASSUME_NONNULL_END
