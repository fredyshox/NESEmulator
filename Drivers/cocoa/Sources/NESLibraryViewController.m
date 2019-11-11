//
//  LibraryViewController.m
//  nescocoa
//
//  Created by Kacper Raczy on 08/11/2019.
//  Copyright © 2019 Kacper Raczy. All rights reserved.
//

#import "NESLibraryViewController.h"
#import "NESGameCollectionViewItem.h"
#import "NESGameWindowController.h"

#define kGameItemIdentifier @"NESGameCollectionViewItem"

@implementation NESLibraryViewController

@synthesize collectionView = _collectionView;
@synthesize games = _games;
@synthesize gameWindowController = _gameWindowController;

- (void)loadView {
    [self setView: [[NSView alloc] init]];
}

- (instancetype)init {
    self = [super initWithNibName: nil bundle: nil];
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    [[self view] setWantsLayer: YES];
    
    _games = [self loadGames];
    
    _collectionView = [[NSCollectionView alloc] init];
    [[self view] addSubview: _collectionView];
    [self setUpCollectionView];
}

- (void)viewDidLayout {
    [super viewDidLayout];
    [_collectionView setFrame: [[self view] frame]];
}

- (void)setUpCollectionView {
    NSCollectionViewFlowLayout* layout = [[NSCollectionViewFlowLayout alloc] init];
    [layout setItemSize: NSMakeSize(160.0, 150.0)];
    [layout setMinimumLineSpacing: 16.0];
    [layout setMinimumInteritemSpacing: 24.0];
    [layout setSectionInset: NSEdgeInsetsMake(24.0, 16.0, 24.0, 16.0)];
    NSNib* itemNib = [[NSNib alloc] initWithNibNamed: @"NESGameCollectionViewItem" bundle: nil];
    [_collectionView registerNib: itemNib forItemWithIdentifier: kGameItemIdentifier];
    [_collectionView setBackgroundColors: @[ [NSColor clearColor] ]];
    [_collectionView setSelectable: YES];
    [_collectionView setCollectionViewLayout: layout];
    [_collectionView setDataSource: self];
    [_collectionView setDelegate: self];
}

- (NSArray*)loadGames {
    NSMutableArray<NESGame*>* gamesMutable = [NSMutableArray array];
    [gamesMutable addObject: [[NESGame alloc] initWithTitle: @"Pac-Man" andPath: [NSURL fileURLWithPath: @"../../testsuite/roms/Pac-Man (U).nes"]]];
    [gamesMutable addObject: [[NESGame alloc] initWithTitle: @"Super Mario Bros" andPath: [NSURL fileURLWithPath: @"../../testsuite/roms/Super Mario Bros (U).nes"]]];
    [gamesMutable addObject: [[NESGame alloc] initWithTitle: @"Donkey Kong Classics" andPath: [NSURL fileURLWithPath: @"../../testsuite/roms/Donkey Kong Classics (U).nes"]]];
    return gamesMutable;
}

- (IBAction)addRom:(id)sender {
    NSOpenPanel* panel = [NSOpenPanel openPanel];
    [panel setCanChooseFiles: YES];
    [panel setCanChooseDirectories: NO];
    [panel setAllowsMultipleSelection: NO];
    [panel setAllowedFileTypes: @[@"nes"]];
    
    [panel beginSheetModalForWindow: [[self view] window] completionHandler:^(NSModalResponse result) {
        if (result == NSModalResponseOK) {
            for (NSURL* url in [panel URLs]) {
                NSLog(@"%@", [url absoluteString]);
            }
        }
    }];
}

// MARK: GameCollectionViewItemDelegate

- (void)gcvItemDidReceiveDeleteGameRequest:(NESGameCollectionViewItem *)item {
    
}

- (void)gcvItemDidReceivePlayGameRequest:(NESGameCollectionViewItem *)item {
    NSIndexPath* indexPath = [_collectionView indexPathForItem: item];
    NESGame* game = [_games objectAtIndex: indexPath.item];
    _gameWindowController = [[NESGameWindowController alloc] initWithWindowNibName: @"NESGameWindowController"];
    [_gameWindowController setGame: game];
    [_gameWindowController showWindow: nil];
    [[_gameWindowController window] makeKeyAndOrderFront: nil];
}

// MARK: NSCollectionViewDelegate & DataSource

- (NSInteger)numberOfSectionsInCollectionView:(NSCollectionView *)collectionView {
    return 1;
}

- (NSInteger)collectionView:(NSCollectionView *)collectionView numberOfItemsInSection:(NSInteger)section {
    return [_games count];
}

- (NSCollectionViewItem *)collectionView:(NSCollectionView *)collectionView itemForRepresentedObjectAtIndexPath:(NSIndexPath *)indexPath {
    NESGameCollectionViewItem* item = [collectionView makeItemWithIdentifier: kGameItemIdentifier forIndexPath: indexPath];
    NESGame* game = [_games objectAtIndex: [indexPath item]];
    [item.textField setStringValue: game.title];
    [item setDelegate: self];
    return item;
}

@end
