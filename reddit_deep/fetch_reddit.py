import json, sys, time, urllib.request, urllib.parse

def fetch_json(url, retries=3):
    """Fetch JSON from Reddit API with retries"""
    headers = {
        'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36'
    }
    for attempt in range(retries):
        try:
            req = urllib.request.Request(url, headers=headers)
            with urllib.request.urlopen(req, timeout=15) as resp:
                return json.loads(resp.read().decode())
        except Exception as e:
            print(f"  Attempt {attempt+1} failed: {e}", file=sys.stderr)
            if attempt < retries-1:
                time.sleep(2 ** attempt)
    return None

def get_top_posts(subreddit, limit=25, min_score=200):
    """Get top posts from a subreddit"""
    url = f"https://www.reddit.com/r/{subreddit}/top.json?t=month&limit={limit}"
    data = fetch_json(url)
    if not data or 'data' not in data:
        return []
    posts = []
    for p in data['data']['children']:
        pd = p['data']
        if pd['score'] >= min_score:
            posts.append({
                'id': pd['id'],
                'title': pd['title'],
                'score': pd['score'],
                'permalink': pd['permalink'],
                'num_comments': pd['num_comments'],
                'selftext': pd.get('selftext', '')
            })
    return posts

def get_comments(post_id, subreddit, limit=20):
    """Get top comments for a post"""
    url = f"https://www.reddit.com/r/{subreddit}/comments/{post_id}/.json?limit={limit}"
    data = fetch_json(url)
    if not data or len(data) < 2:
        return [], {}
    
    post_data = data[0]['data']['children'][0]['data']
    post_info = {
        'title': post_data.get('title', ''),
        'score': post_data.get('score', 0),
        'selftext': post_data.get('selftext', ''),
        'author': post_data.get('author', ''),
        'created_utc': post_data.get('created_utc', 0),
    }
    
    comments = []
    for c in data[1]['data']['children']:
        cd = c['data']
        comments.append({
            'id': cd.get('id', ''),
            'author': cd.get('author', ''),
            'body': cd.get('body', ''),
            'score': cd.get('score', 0),
            'created_utc': cd.get('created_utc', 0),
            'parent_id': cd.get('parent_id', ''),
            'is_submitter': cd.get('is_submitter', False),
        })
    return comments, post_info

def search_posts_by_keyword(subreddit, keyword, limit=10):
    """Search for posts by keyword in title"""
    url = f"https://www.reddit.com/r/{subreddit}/search.json?q={urllib.parse.quote(keyword)}&sort=top&restrict_sr=1&limit={limit}"
    data = fetch_json(url)
    if not data or 'data' not in data:
        return []
    results = []
    for p in data['data']['children']:
        pd = p['data']
        results.append({
            'id': pd['id'],
            'title': pd['title'],
            'score': pd['score'],
            'permalink': pd['permalink'],
        })
    return results

# Target posts we know about
target_posts = {
    'TrollCoping': [
        {'id': '1sktzwh', 'title': 'Is self awareness bad?', 'score': 3934},
        {'id': '1sj3fkx', 'title': 'Misandrists Make Me Upsetttt', 'score': 2120},
    ],
    'PoliticalCompassMemes': [
        {'title': 'You Will Own Nothing and Be Happy', 'keyword': 'You Will Own Nothing and Be Happy'},
    ],
    'FeMRADebates': [
        {'title': 'Lets talk TERFs', 'keyword': 'Lets talk TERFs'},
    ],
    'Feminism': [
        {'title': 'Why do TERFs get so angry', 'keyword': 'Why do TERFs get so angry'},
    ],
    'LeftWingMaleAdvocates': [
        {'title': 'A friendly reminder the F in TERF stands for feminist', 'keyword': 'F in TERF stands for feminist'},
        {'title': 'Journey Updates from (Extremely Former) RadFem Mom', 'keyword': 'Journey Updates from'},
    ],
    'GenderCynical': [
        {'title': 'Welcome to Reddit\'s most active feminist community', 'keyword': 'Welcome to Reddit most active feminist'},
    ],
    'AskFeminists': [
        {'title': 'Are there any feminists you do not consider allies?', 'keyword': 'feminists you do not consider allies'},
    ],
    'MensRights': [
        {'id': '1s1k5g5', 'title': 'Yes, feminism is misandry', 'score': 424},
    ],
}

all_data = {}

# Fetch posts from each target subreddit
for subreddit in ['TrollCoping', 'PoliticalCompassMemes', 'FeMRADebates', 'LeftWingMaleAdvocates', 'MensRights', 'AskFeminists', 'Feminism', 'GenderCynical']:
    print(f"\n=== Fetching r/{subreddit} ===")
    all_data[subreddit] = {'posts': []}
    
    # Get top posts from the subreddit
    top_posts = get_top_posts(subreddit, limit=30, min_score=100)
    print(f"  Found {len(top_posts)} posts with score >= 100")
    
    # Find target posts
    targets = target_posts.get(subreddit, [])
    
    for post_info in targets:
        post_id = post_info.get('id')
        keyword = post_info.get('keyword', '')
        
        if not post_id and keyword:
            # Search for the post
            found = search_posts_by_keyword(subreddit, keyword, limit=5)
            if found:
                post_id = found[0]['id']
                print(f"  Found via search: {found[0]['title'][:50]} (score: {found[0]['score']})")
        
        if post_id:
            print(f"  Fetching comments for post {post_id}...")
            comments, post_data = get_comments(post_id, subreddit, limit=20)
            if comments:
                all_data[subreddit]['posts'].append({
                    'id': post_id,
                    'title': post_data.get('title', ''),
                    'score': post_data.get('score', 0),
                    'selftext': post_data.get('selftext', ''),
                    'author': post_data.get('author', ''),
                    'num_comments': len(comments),
                    'comments': comments
                })
                print(f"    Got {len(comments)} comments")
            time.sleep(1)

# Save results
output_path = '/home/alca/reddit_deep/threads_full.json'
with open(output_path, 'w') as f:
    json.dump(all_data, f, indent=2)

print(f"\n=== SAVED to {output_path} ===")
# Print summary
total_posts = sum(len(d['posts']) for d in all_data.values())
total_comments = sum(sum(len(p['comments']) for p in d['posts']) for d in all_data.values())
print(f"Total: {total_posts} posts, {total_comments} comments")
