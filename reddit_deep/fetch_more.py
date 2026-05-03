import json, sys, time, urllib.request, urllib.parse

def fetch_json(url, retries=3):
    headers = {
        'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36'
    }
    for attempt in range(retries):
        try:
            req = urllib.request.Request(url, headers=headers)
            with urllib.request.urlopen(req, timeout=20) as resp:
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

def get_top_posts(subreddit, limit=25, min_score=100):
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
            })
    return posts

# Load existing data
with open('/home/alca/reddit_deep/threads_full.json', 'r') as f:
    existing = json.load(f)

# More specific target searches with exact title matching
additional_targets = [
    ('TrollCoping', 'Yes, feminism is misandry', 'yes feminism is misandry'),
    ('TrollCoping', 'Misandrists Make Me Upsetttt', 'Misandrists Make Me'),
    ('Feminism', 'Why do TERFs get so angry', 'Why do TERFs'),
    ('FeMRADebates', 'Lets talk TERFs', 'lets talk terfs'),
    ('LeftWingMaleAdvocates', 'A friendly reminder the F in TERF', 'F in TERF'),
    ('MensRights', 'Yes, feminism is misandry', 'yes feminism is misandry'),
]

for subreddit, title, query in additional_targets:
    print(f"\n=== Searching r/{subreddit} for: {query} ===")
    url = f"https://www.reddit.com/r/{subreddit}/search.json?q={urllib.parse.quote(query)}&sort=top&restrict_sr=1&limit=10"
    data = fetch_json(url)
    if data and 'data' in data:
        found = False
        for p in data['data']['children']:
            pd = p['data']
            title_lower = pd['title'].lower()
            query_lower = query.lower()
            if query_lower in title_lower or title_lower.replace(' ', '').replace('-','').replace('\'','') in query_lower.replace(' ', '').replace('-','').replace('\'',''):
                print(f"  Found: {pd['title'][:70]} (id: {pd['id']}, score: {pd['score']})")
                # Check if we already have this post
                already_have = any(post.get('id') == pd['id'] for sub_data in existing.values() for post in sub_data.get('posts', []))
                if not already_have:
                    comments, post_info = get_comments(pd['id'], subreddit, 25)
                    if comments:
                        if subreddit not in existing:
                            existing[subreddit] = {'posts': []}
                        existing[subreddit]['posts'].append({
                            'id': pd['id'],
                            'title': pd['title'],
                            'score': pd['score'],
                            'selftext': pd.get('selftext', ''),
                            'comments': comments
                        })
                        print(f"    Added {len(comments)} comments")
                        found = True
                else:
                    print(f"    Already have this post")
        if not found:
            print("  No exact match found")
    time.sleep(1)

# Now fetch additional top posts from each subreddit to add more content
for subreddit in ['TrollCoping', 'PoliticalCompassMemes', 'FeMRADebates', 'LeftWingMaleAdvocates', 'MensRights', 'AskFeminists', 'Feminism', 'GenderCynical']:
    print(f"\n=== Getting more top posts from r/{subreddit} ===")
    posts = get_top_posts(subreddit, limit=25, min_score=200)
    existing_ids = [post.get('id') for sub_data in existing.values() for post in sub_data.get('posts', [])]
    new_posts = [p for p in posts if p['id'] not in existing_ids][:5]  # Add up to 5 new posts
    for p in new_posts:
        print(f"  Adding: {p['title'][:60]} (score: {p['score']})")
        comments, post_info = get_comments(p['id'], subreddit, 25)
        if comments:
            if subreddit not in existing:
                existing[subreddit] = {'posts': []}
            existing[subreddit]['posts'].append({
                'id': p['id'],
                'title': post_info.get('title', ''),
                'score': post_info.get('score', 0),
                'selftext': post_info.get('selftext', ''),
                'comments': comments
            })
        time.sleep(0.5)
    print(f"  Added {len(new_posts)} new posts from r/{subreddit}")

# Save updated data
with open('/home/alca/reddit_deep/threads_full.json', 'w') as f:
    json.dump(existing, f, indent=2)

# Summary
total_posts = sum(len(d['posts']) for d in existing.values())
total_comments = sum(sum(len(p['comments']) for p in d['posts']) for d in existing.values())
print(f"\n=== FINAL: {total_posts} posts, {total_comments} comments ===")
print("\nBy subreddit:")
for sub, data in existing.items():
    post_count = len(data['posts'])
    comment_count = sum(len(p['comments']) for p in data['posts'])
    print(f"  {sub}: {post_count} posts, {comment_count} comments")
