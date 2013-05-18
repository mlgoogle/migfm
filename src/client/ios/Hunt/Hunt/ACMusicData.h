//
//  ACMusicData.h
//  Hunt
//
//  Created by Archer_LJ on 13-3-9.
//  Copyright (c) 2013年 Archer_LJ. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface ACMusicData : NSObject
{
    NSString* _name;
    NSString* _artist;
}

@property (nonatomic, assign) NSString* name;
@property (nonatomic, assign) NSString* artist;

@end
