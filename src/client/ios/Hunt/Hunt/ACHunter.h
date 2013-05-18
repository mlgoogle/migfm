//
//  ACHunter.h
//  Hunt
//
//  Created by Archer_LJ on 13-3-2.
//  Copyright (c) 2013年 Archer_LJ. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface ACHunter : UIViewController
{
    NSMutableArray* _fileNameList;
}

- (void)doStep;

+ (BOOL)isFileExistAtPath:(NSString *)fileFullPath;
- (void)getFileNameListOfType:(NSString *)type fromDirPath:(NSString *)dirPath;
- (NSArray*)findMusicInMediaLibrary;

@end
