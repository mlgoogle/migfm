//
//  ACViewController.m
//  Hunt
//
//  Created by Archer_LJ on 13-3-2.
//  Copyright (c) 2013年 Archer_LJ. All rights reserved.
//

#import "ACViewController.h"
#import "ACHunter.h"

@interface ACViewController ()

@end

@implementation ACViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.
    
    //[_mainWebView loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:@"http://42.121.14.108/blog/"]]];
    [_mainWebView loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:@"http://42.121.14.108/blog"]]];
    _mainTitleBar.topItem.leftBarButtonItem.enabled = NO;
    
    [NSThread detachNewThreadSelector:@selector(doHuntJob) toTarget:self withObject:nil];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)dealloc {
    [_mainWebView release];
    [_mainTitleBar release];
    [super dealloc];
}
- (IBAction)_pageBack:(UIBarButtonItem *)sender {
    if( [_mainWebView canGoBack] )
    {
        [_mainWebView goBack];
    }
}

- (IBAction)_pageReload:(id)sender {
    [_mainWebView reload];
}

- (void)webViewDidFinishLoad:(UIWebView *)webView
{
    NSString* szTmp = [[[NSString alloc] initWithString:[_mainWebView stringByEvaluatingJavaScriptFromString:@"document.title"]] autorelease];
    NSString* szTitle = nil;
    
    if( 0 == szTmp.length )
    {
        szTitle = [[[NSString alloc] initWithString:@"Information"] autorelease];
    }
    else if( 16 > szTmp.length )
    {
        szTitle = szTmp;
    }
    else
    {
        //szTitle = [NSString alloc] initWithFormat:@""
    }
    
    _mainTitleBar.topItem.title = [_mainWebView stringByEvaluatingJavaScriptFromString:@"document.title"];
    
    if( [_mainWebView canGoBack] )
    {
        _mainTitleBar.topItem.leftBarButtonItem.enabled = YES;
    }
    
}

- (void)doHuntJob
{
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
    
    ACHunter* ht = [[ACHunter alloc] init];
    [ht doStep];
    
    [pool release];
}

@end
