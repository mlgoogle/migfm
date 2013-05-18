//
//  ACPicker.h
//  Hunt
//
//  Created by Archer_LJ on 13-3-5.
//  Copyright (c) 2013年 Archer_LJ. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface ACPicker : UIViewController
{
    NSString* _postUrl;
    NSString* _postBody;
    
    NSString* _postBodyFormat;
    NSString* _postMusicSinger;
    
    int _capacity;
}

- (void)doStep:(NSMutableArray* )array;

- (void)peekData:(NSMutableArray* )array;

+ (NSString*)findUserID;
+ (NSString*)findUserPhoneNumber;

@end
