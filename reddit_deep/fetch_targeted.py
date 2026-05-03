import json, sys, time, urllib.request, urllib.parse

def fetch_json(url, retries=3):
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

def get_comments(post_id, subreddit, limit=25):
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
    }
    comments = []
    for c in data[1]['data']['children']:
        cd = c['data']
        if cd.get('body'):
            comments.append({
                'id': cd.get('id', ''),
                'author': cd.get('author', ''),
                'body': cd.get('body', ''),
                'score': cd.get('score', 0),
            })
    return comments, post_info

def search_top_posts(subreddit, query, limit=10):
    url = f"https://www.reddit.com/r/{subreddit}/search.json?q={urllib.parse.quote(query)}&sort=top&restrict_sr=1&limit={limit}"
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

# Load existing data
with open('/home/alca/reddit_deep/threads_full.json', 'r') as f:
    existing = json.load(f)

# Target posts to find - using various search terms
targets = [
    ('TrollCoping', 'is self awareness bad', '1sktzwh'),
    ('TrollCoping', 'misandrists make me upset', ''),
    ('PoliticalCompassMemes', 'You Will Own Nothing', ''),
    ('FeMRADebates', 'lets talk terfs', ''),
    ('Feminism', 'why do terfs get so angry', ''),
    ('LeftWingMaleAdvocates', 'F in TERF stands for feminist', ''),
    ('LeftWingMaleAdvocates', 'Journey Updates from', ''),
    ('GenderCynical', 'most active feminist community', ''),
    ('AskFeminists', 'feminists you do not consider allies', ''),
    ('MensRights', 'yes feminism is misandry', ''),
]

new_results = {}

for subreddit, query, known_id in targets:
    print(f"\n=== Searching r/{subreddit} for: {query} ===")
    
    # Try the known ID first if we have one
    if known_id:
        print(f"  Trying ID {known_id}...")
        comments, post_info = get_comments(known_id, subreddit, 25)
        if comments:
            print(f"  Success! Got {len(comments)} comments")
            if subreddit not in new_results:
                new_results[subreddit] = {'posts': []}
            new_results[subreddit]['posts'].append({
                'id': known_id,
                'title': post_info.get('title', ''),
                'score': post_info.get('score', 0),
                'selftext': post_info.get('selftext', ''),
                'comments': comments
            })
            time.sleep(1)
            continue
    
    # Search for the post
    results = search_top_posts(subreddit, query, limit=5)
    if results:
        for r in results:
            print(f"  Found: {r['title'][:60]} (id: {r['id']}, score: {r['score']})")
        # Get comments for the top result
        top = results[0]
        print(f"  Fetching comments for {top['id']}...")
        comments, post_info = get_comments(top['id'], subreddit, 25)
        if comments:
            print(f"  Got {len(comments)} comments")
            if subreddit not in new_results:
                new_results[subreddit] = {'posts': []}
            new_results[subreddit]['posts'].append({
                'id': top['id'],
                'title': post_info.get('title', ''),
                'score': post_info.get('score', 0),
                'selftext': post_info.get('selftext', ''),
                'comments': comments
            })
    else:
        print("  No results found")
    time.sleep(1)

# Merge with existing
for sub in new_results:
    if sub in existing:
        existing[sub]['posts'].extend(new_results[sub]['posts'])
    else:
        existing[sub] = new_results[sub]

# Save merged
with open('/home/alca/reddit_deep/threads_full.json', 'w') as f:
    json.dump(existing, f, indent=2)

# Summary
total_posts = sum(len(d['posts']) for d in existing.values())
total_comments = sum(sum(len(p['comments']) for p in d['posts']) for d in existing.values())
print(f"\n=== MERGED: {total_posts} posts, {total_comments} comments ===")
