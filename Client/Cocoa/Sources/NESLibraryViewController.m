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
#define kLibraryDirectory @"Game Library"
#define kLibraryRomsDirectory @"Roms"
#define kLibraryDatabaseFileName @"GameLibrary.sqlite3"

@implementation NESLibraryViewController

- (void)loadView {
    [self setView: [[NSView alloc] init]];
}

- (instancetype)init {
    if (self = [super initWithNibName: nil bundle: nil]) {
        [self initLibraryDirs];
        _library = [[NESLibrary alloc] initWithPath: [_databaseURL path]];
    }
    return self;
}

// MARK: VC Lifecycle

- (void)viewDidLoad {
    [super viewDidLoad];
    [[self view] setWantsLayer: YES];
    NSNotificationCenter* center = [NSNotificationCenter defaultCenter];
    [center addObserver: _library
               selector: @selector(closeDatabase)
                   name: NSApplicationWillTerminateNotification
                 object: nil];
    
    _games = [_library loadGames];
    _scrollView = [[NSScrollView alloc] init];
    _collectionView = [[NSCollectionView alloc] init];
    [self setUpCollectionView];
    [_scrollView setDocumentView: _collectionView];
    [[self view] addSubview: _scrollView];
}

- (void)viewDidLayout {
    [super viewDidLayout];
    [_scrollView setFrame: [[self view] frame]];
}

// MARK: Setup

- (void)setUpCollectionView {
    NSCollectionViewFlowLayout* layout = [[NSCollectionViewFlowLayout alloc] init];
    [layout setItemSize: NSMakeSize(160.0, 150.0)];
    [layout setMinimumLineSpacing: 16.0];
    [layout setMinimumInteritemSpacing: 24.0];
    [layout setSectionInset: NSEdgeInsetsMake(24.0, 16.0, 24.0, 16.0)];
    [layout setScrollDirection: NSCollectionViewScrollDirectionVertical];
    NSNib* itemNib = [[NSNib alloc] initWithNibNamed: @"NESGameCollectionViewItem" bundle: nil];
    [_collectionView registerNib: itemNib forItemWithIdentifier: kGameItemIdentifier];
    [_collectionView setBackgroundColors: @[ [NSColor clearColor] ]];
    [_collectionView setSelectable: YES];
    [_collectionView setAllowsMultipleSelection: NO];
    [_collectionView setCollectionViewLayout: layout];
    [_collectionView setDataSource: self];
    [_collectionView setDelegate: self];
}

- (NSURL* _Nullable) applicationSupportDirWithError: (NSError**) error {
    NSFileManager* manager = [NSFileManager defaultManager];
    NSURL* applicationSupport = [manager URLForDirectory: NSApplicationSupportDirectory inDomain: NSUserDomainMask appropriateForURL: nil create: false error: error];
    NSURL* dir = [applicationSupport URLByAppendingPathComponent: [[NSBundle mainBundle] bundleIdentifier]];
    BOOL success = [manager createDirectoryAtURL: dir withIntermediateDirectories: YES attributes: nil error: error];
    
    return success ? dir : nil;
}

/**
 Creates files & dirs for app in application support directory
 */
- (void)initLibraryDirs {
    NSError* error = nil;
    NSFileManager* manager = [NSFileManager defaultManager];
    NSURL* asDir = [self applicationSupportDirWithError: &error];
    if (asDir == nil) {
        NSLog(@"Error cannot locate application support dir: %@", [error localizedDescription]);
        return;
    }
    
    _libraryDir = [asDir URLByAppendingPathComponent: kLibraryDirectory];
    _romsDir = [_libraryDir URLByAppendingPathComponent: kLibraryRomsDirectory];
    _databaseURL = [_libraryDir URLByAppendingPathComponent: kLibraryDatabaseFileName];
    // This will create both dirs (immediate dirs to YES)
    BOOL success = [manager createDirectoryAtURL: _romsDir withIntermediateDirectories: YES attributes: nil error: &error];
    if (!success) {
        NSLog(@"Error while creating library dirs: %@", [error localizedDescription]);
        return;
    }
    
    if (![manager fileExistsAtPath: [_databaseURL path]]) {
        [manager createFileAtPath: [_databaseURL path] contents: [NSData data] attributes: nil];
    }
}

// MARK: Actions

- (IBAction)addRom:(id)sender {
    NSOpenPanel* panel = [NSOpenPanel openPanel];
    [panel setCanChooseFiles: YES];
    [panel setCanChooseDirectories: NO];
    [panel setAllowsMultipleSelection: NO];
    [panel setAllowedFileTypes: @[@"nes"]];
    
    [panel beginSheetModalForWindow: [[self view] window] completionHandler:^(NSModalResponse result) {
        if (result == NSModalResponseOK) {
            NSURL* url = [[panel URLs] lastObject];
            NSString* title = [self titleFromURL: url];
            NESGame* game = [[NESGame alloc] initWithId: -1 andTitle: title andPath: url];
            __weak __typeof(self)weakSelf = self;
            dispatch_async(dispatch_get_main_queue(), ^{
                __strong __typeof(weakSelf)strongSelf = weakSelf;
                NSError* error;
                if (![[strongSelf library] addGame: game error: &error]) {
                    NSLog(@"Error: %@", [error localizedDescription]);
                    return;
                }
                
                [strongSelf reloadData];
            });
        }
    }];
}

- (NSString*) titleFromURL: (NSURL*) url {
    return [[url URLByDeletingPathExtension] lastPathComponent];
}

- (void) reloadData {
    _games = [_library loadGames];
    [[_collectionView animator] reloadData];
}

// MARK: GameCollectionViewItemDelegate

- (void)gcvItemDidReceiveDeleteGameRequest:(NESGameCollectionViewItem *)item {
    NSIndexPath* indexPath = [_collectionView indexPathForItem: item];
    NESGame* game = [_games objectAtIndex: [indexPath item]];
    __weak __typeof(self)weakSelf = self;
    dispatch_async(dispatch_get_main_queue(), ^{
        __strong __typeof(weakSelf)strongSelf = weakSelf;
        NSError* error;
        if (![[strongSelf library] removeGame: game error: &error]) {
            NSLog(@"Error: %@", [error localizedDescription]);
            return;
        }
        
        [strongSelf reloadData];
    });
}

- (void)gcvItemDidReceivePlayGameRequest:(NESGameCollectionViewItem *)item {
    NSIndexPath* indexPath = [_collectionView indexPathForItem: item];
    NESGame* game = [_games objectAtIndex: indexPath.item];
    _gameWindowController = [[NESGameWindowController alloc] initWithWindowNibName: @"NESGameWindowController"];
    [_gameWindowController setGame: game];
    [_gameWindowController showWindow: nil];
    [[_gameWindowController window] makeKeyAndOrderFront: nil];
}

- (void)gcvItemDidReceiveShowInFinderRequest:(NESGameCollectionViewItem *)item {
    NSIndexPath* indexPath = [_collectionView indexPathForItem: item];
    NESGame* game = [_games objectAtIndex: indexPath.item];
    NSArray<NSURL*>* urls = @[game.path];
    [[NSWorkspace sharedWorkspace] activateFileViewerSelectingURLs: urls];
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
