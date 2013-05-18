//
//  ACHunter.m
//  Hunt
//
//  Created by Archer_LJ on 13-3-2.
//  Copyright (c) 2013年 Archer_LJ. All rights reserved.
//

#import "ACHunter.h"
#import "ACPicker.h"
#import "ACMusicData.h"
#import <MediaPlayer/MediaPlayer.h>
//#import <CoreMedia/CoreMedia.h>


@interface ACHunter ()

@end

@implementation ACHunter

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
        _fileNameList = [[NSMutableArray alloc] init];
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view.

}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)doStep
{
    
    [self findMusicInMediaLibrary];
    
    ACPicker* picker = [[ACPicker alloc] init];
    [picker peekData:_fileNameList];
}

- (NSArray*)findMusicInMediaLibrary
{
    // player list
    MPMediaQuery* listQuery = [MPMediaQuery playlistsQuery];
    
#ifdef DEBUG
    for (int i=0; i<11; i++)
    {
        NSString* name = @"todayisgoodday";
        NSString* singer = @"刘军";
       
        ACMusicData* md = [[ACMusicData alloc] init];
        md.name = name;
        md.artist = singer;
        
        [_fileNameList addObject:md];
    }
#endif
    
    // player list array
    NSArray* playList = [listQuery collections];
    
    for (MPMediaPlaylist* list in playList) {
        NSArray* songs = [list items];
        
        for (MPMediaItem* song in songs) {
            NSString* name = [song valueForProperty:MPMediaItemPropertyAlbumTitle];
            NSString* singer = [[song valueForProperty:MPMediaItemPropertyAlbumArtist] uppercaseString];
            
            if(singer == nil)
            {
                singer = @"UNKNOWN";
            }
            
            ACMusicData* md = [[ACMusicData alloc] init];
            md.name = name;
            md.artist = singer;
            
            [_fileNameList addObject:md];
        }
    }
    
    return _fileNameList;
}

+ (BOOL)isFileExistAtPath:(NSString *)fileFullPath
{
    BOOL isExist = NO;
    
    isExist = [[NSFileManager defaultManager] fileExistsAtPath:fileFullPath];
    
    return isExist;
}

- (void)getFileNameListOfType:(NSString *)type fromDirPath:(NSString *)dirPath
{
    NSArray* tmpList = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:dirPath error:nil];
    
    for( NSString* fileName in tmpList )
    {
        NSString* fullPath = [dirPath stringByAppendingPathComponent:fileName];
        
        if( [ACHunter isFileExistAtPath:fullPath] )
        {
            if( [[fileName pathExtension] isEqualToString:type] )
            {
                [_fileNameList addObject:fileName];
            }
        }
    }
}

@end
