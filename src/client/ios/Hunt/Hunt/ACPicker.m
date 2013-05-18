//
//  ACPicker.m
//  Hunt
//
//  Created by Archer_LJ on 13-3-5.
//  Copyright (c) 2013年 Archer_LJ. All rights reserved.
//

#import "ACPicker.h"
#import "ACMusicData.h"

@interface ACPicker ()

@end

@implementation ACPicker

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
        _postUrl = [[[NSString alloc] initWithString:@"http://42.121.112.248/cgi-bin/record.fcgi"] autorelease];
        
        _capacity = 10;
        
        // %@ music name, %@ singer name
        _postMusicSinger = [[[NSString alloc] initWithString:@"{\"name:\":\"%@\",\"singer\":\"%@\"}"] autorelease];
        
        // %d enter, %d urlcode, %@mail, %@ phone, %d enter, 
        _postBodyFormat = [[[NSString alloc] initWithString:@"enter=%d&urlcode=%d&content={\"name\":\"%@\",\"phone\":\"%@\",\"enter\":\"%d\",\"music\":[%@]}"] autorelease];
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

- (void)doStep:(NSMutableArray* )array
{
    [self peekData:array];
}

+ (NSString*)findUserID
{
    NSString* userID = [[[NSString alloc] init] autorelease];
    
    // do finding job
    
    if( userID == nil )
    {
        userID = @"UNKNOWN";
    }
    
    return userID;
}

+ (NSString*)findUserPhoneNumber
{
    NSString* phone = [[[NSString alloc] init] autorelease];
    
    // do finding job
    
    if( phone == nil )
    {
        phone = @"00000000000";
    }
    
    return phone;
}

- (void)peekData:(NSMutableArray *)array
{
    int repeatTime = [array count] / _capacity;
    int lastTime = [array count] % _capacity;
    
#ifdef DEBUG
    repeatTime = 1;
    lastTime = 0;
#endif
    
    for (int i=0; i<repeatTime; i++) {
        NSMutableString* allMusic = [[[NSMutableString alloc] init] autorelease];
        
        for (int j=0; j<_capacity; j++)
        {
            ACMusicData* md = [[ACMusicData alloc] init];
            md = [array objectAtIndex:j];
            
            NSString* single = [[NSString alloc] initWithFormat:_postMusicSinger, md.name, md.artist];
            
            if (j!=0)
            {
                [allMusic appendString:@","];
            }
            
            [allMusic appendString:single];
        }
        
        NSString* userID = [ACPicker findUserID];
        NSString* phone = [ACPicker findUserPhoneNumber];
        
        NSString* post = [[[NSString alloc] initWithFormat:_postBodyFormat, 2, 1, userID, phone, 2, allMusic ] autorelease];
        
        NSData* postData = [post dataUsingEncoding:NSUTF8StringEncoding allowLossyConversion:YES];
        NSMutableURLRequest* request = [[[NSMutableURLRequest alloc] init] autorelease];
        
        [request setURL:[NSURL URLWithString:_postUrl]];
        [request setHTTPMethod:@"POST"];
        [request setHTTPBody:postData];
        
        NSURLConnection* conn = [[NSURLConnection alloc] initWithRequest:request delegate:self];
        
        if( conn )
        {
        
        }
    }
    
    for (int i=0; i<lastTime; i++)
    {
    
    }
}

@end
