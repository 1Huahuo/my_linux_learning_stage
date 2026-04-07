# Day44

## 106.[[Easy+]112. 路径总和](https://leetcode.cn/problems/path-sum/description/)——”DFS-前序遍历“

```c
// DFS

// 写法1：前序遍历-累加
// bool hasPathSum(struct TreeNode* root, int targetSum) {
//     bool is_equal = false;

//     void dfs(struct TreeNode* root, int sum){
//         if(!root)
//             return;

//         sum += root->val;
//         if(!root->left && !root->right && sum == targetSum)
//             is_equal = true;

//         dfs(root->left, sum);
//         dfs(root->right, sum);
//     }

//     dfs(root, 0);
//     return is_equal;
// }

// 写法2：前序遍历-累减
bool hasPathSum(struct TreeNode* root, int targetSum) {
    if(!root)
        return false;

    targetSum -= root->val;
    if(!root->left && !root->right)
        return targetSum == 0;

    return hasPathSum(root->left, targetSum) || hasPathSum(root->right, targetSum);
}
```

## 107.[[Medium]113. 路径总和 II](https://leetcode.cn/problems/path-sum-ii/description/)——“DFS-前序遍历”&==“回溯”==

```c
// DFS-前序遍历&回溯
int** pathSum(struct TreeNode* root, int targetSum, int* returnSize, int** returnColumnSizes) {
    int** ans = (int**)malloc(2500 * sizeof(int*)); // 5000个结点最多有2500个叶结点
    int* ansColSize = (int*)malloc(2500 * sizeof(int));
    int ansSize = 0;

    int* path = (int*)malloc(2500 * sizeof(int));
    int pathSize = 0;

    void dfs(struct TreeNode* root, int targetSum){
        if(!root)
            return;
        path[pathSize++] = root->val;
        targetSum -= root->val; // 112.前序遍历-累减
        if(!root->left && !root->right && targetSum == 0){
            int* tmp = (int*)malloc(pathSize * sizeof(int));
            memcpy(tmp, path, pathSize * sizeof(int));
            ans[ansSize] = tmp;
            ansColSize[ansSize++] = pathSize; // 嵌套函数中可以直接用*returnSize，因为其相对于此函数来说是全局变量
        }
        dfs(root->left, targetSum);
        dfs(root->right, targetSum);
        pathSize--; // 回溯、(相对)全局
    }

    dfs(root, targetSum);
    *returnColumnSizes = ansColSize;
    *returnSize = ansSize;
    return ans;
}
```

## 108.[[Medium+]437. 路径总和 III](https://leetcode.cn/problems/path-sum-iii/description/)——==“回溯”==&“哈希表-uthash”&“前缀和”&“DFS”

```c
// 法一：回溯&哈希表&前缀和&DFS
typedef struct{
    long long key; // 前缀和，pre[i]表示从[0...i-1]的元素之和，在这里表示从根到node的父结点的结点值之和(node的结点值尚未计入)
    int count; // 该前缀和出现的次数
    UT_hash_handle hh;
}Hash;

typedef struct{
    Hash* hashTable; // 哈希表本体
    int targetSum;
    int ans;
}Context; // 用于在递归函数中传递参数的上下文结构体

void dfs(struct TreeNode* root, long long sum, Context* ctx){
    if(!root)
        return;

    sum += root->val;

    // 查找sum - targerSum的出现次数
    Hash *tmp = NULL;
    long long targetKey = sum - ctx->targetSum;
    HASH_FIND(hh, ctx->hashTable, &targetKey, sizeof(long long), tmp);
    if(tmp)
        ctx->ans += tmp->count;

    // 在查找之后再将当前的前缀和(属于是当前结点的后一个结点的)加入哈希表，这个前缀和是供后续子节点作为"起点"使用的
    HASH_FIND(hh, ctx->hashTable, &sum, sizeof(long long), tmp); // tmp的值无影响，因为其作为输出指针，在调用后其值一定会被改变
    if(tmp) // 已存在，增加计数
        tmp->count++;
    else{ // 如果不存在，创建新结点并加入哈希表
        tmp = (Hash*)malloc(sizeof(Hash));
        tmp->key = sum;
        tmp->count = 1;
        HASH_ADD(hh, ctx->hashTable, key, sizeof(long long), tmp);
    }

    // 递归处理左右子树
    dfs(root->left, sum, ctx);
    dfs(root->right, sum, ctx);
        
    // 回溯
    HASH_FIND(hh, ctx->hashTable, &sum, sizeof(long long), tmp);
    tmp->count--;
}

int pathSum(struct TreeNode* root, int targetSum) {
    // 初始化上下文结构体
    Context ctx;
    ctx.hashTable = NULL;
    ctx.targetSum = targetSum;
    ctx.ans = 0;

    // 初始化哈希表，假如空前缀和，出现次数为1，这是为了统一特殊情况，所以前缀和的第一个值为0，代表不包含任何元素的和
    Hash *initNode = (Hash*)malloc(sizeof(Hash));
    initNode->key = 0;
    initNode->count = 1;
    HASH_ADD(hh, ctx.hashTable, key, sizeof(long long), initNode);
    
    dfs(root, 0LL, &ctx);

    Hash* cur, * tmp;
    HASH_ITER(hh, ctx.hashTable, cur, tmp){
        HASH_DEL(ctx.hashTable, cur);
        free(cur);
    }

    return ctx.ans;
}


/*
// 法二：暴力回溯&DFS
int pathSum(struct TreeNode* root, int targetSum) {
    int ans=0;
    int* path=(int*)malloc(1e4 * sizeof(int));
    int pathSize=0;

    void BTK(struct TreeNode *root, int targetSum){
        if(!root)
            return;

        path[pathSize++]=root->val;

        //从后往前求和
        long long sum=0;
        for(int i = pathSize-1; i >= 0; i--){
            sum += path[i];
            if(sum == targetSum)
                ans++;
        }
        BTK(root->left, targetSum);
        BTK(root->right, targetSum);

        pathSize--; // 回溯
    }

    BTK(root, targetSum);
    return ans;
}
*/
```

## 109.[[⭐Medium+]236. 二叉树的最近公共祖先](https://leetcode.cn/problems/lowest-common-ancestor-of-a-binary-tree/description/)——“DFS”

<img src="..\0.TyporaPicture\image-20260407165151536.png" alt="image-20260407165151536" style="zoom: 67%;" />

```c
// DFS

/*
分类讨论
├── 第一类情况（直接返回当前节点）
│   ├── 当前节点是空节点
│   ├── 当前节点是 p
│   └── 当前节点是 q
│   └── 统一处理：返回当前节点
└── 第二类情况（其它，需递归判断）
    ├── 左右子树都找到：返回当前节点
    ├── 只有左子树找到：返回递归左子树的结果
    ├── 只有右子树找到：返回递归右子树的结果
    └── 左右子树都没有找到：返回空节点
*/
struct TreeNode* lowestCommonAncestor(struct TreeNode* root, struct TreeNode* p, struct TreeNode* q){
    if(!root || root == p || root == q)
        return root; // 找到p或q就停止向下递归
    
    struct TreeNode* left = lowestCommonAncestor(root->left, p, q);
    struct TreeNode* right = lowestCommonAncestor(root->right, p, q);

    if(left && right) // 若是左右都找到，则一定在从下向上最近的公共祖先结点第一次满足这个条件，则直接一直返回此结点（第一次在这里返回，之后都是在最下面一行代码返回）
        return root;

    return left ? left : right; // 哪个找到返回哪个（且最多只有一个不为NULL，因为在这行代码之上已经判断过均为真的情况了，所以到达这里时最多只有一个真）
}
```

## 110.[[Hard]124. 二叉树中的最大路径和](https://leetcode.cn/problems/binary-tree-maximum-path-sum/description/)——“树形DP“&”DFS“

```c
// 树形DP&DFS
#define MY_MAX(a, b) ((a) > (b) ? (a) : (b))

int dfs(struct TreeNode* root, int* ans){ // 注意：dfs返回的是“链”的结点值之和，而不是直径的结点值之和，即直径的一半，所以在主函数中不用dfs的结果直接赋值给ans，而是将ans的地址传入函数来不断更新其值
    if(!root)
        return 0; // 即当前没有结点，所以以其为根的子树和为0

    int left_val = dfs(root->left, ans); // 左子树最大"链"和，因为最终要和某个子树的根结点和另一个右子树的最大链拼接成直径，所以若是返回值是左子树的最大直径和则没有意义
    int right_val = dfs(root->right, ans); // 右子树的最大"链"和
    *ans = MY_MAX(*ans, left_val + right_val + root->val);
    return MY_MAX(MY_MAX(left_val,right_val)+root->val , 0); // 从当前子树的左右子树中选出一个更大的左/右子树链和与当前根结点相加，再用这个和与0相比取较大值返回，因为若是当前返回的最大链和是一个负值，则对之后的最大链和更新是副作用，所以应舍弃，即用0值来代替这个子树，即意为这个子树不存在可能的局部最大链和，所以最终的答案链不应该经过这个子树
}

int maxPathSum(struct TreeNode* root) {
    // int ans = 0;
    // 找最大则初始化为最小值，在本题中结点值可以为负，所以应取INT_MIN作为初始值
    int ans = INT_MIN;
    dfs(root, &ans);
    return ans;
}
```

## 111.[[Medium]33. 搜索旋转排序数组](https://leetcode.cn/problems/search-in-rotated-sorted-array/description/)——“二分-闭区间/开区间”

```c
// 二分-闭区间&排除法
// int search(int* nums, int numsSize, int target) {
//     int left = 0, right = numsSize -1;
//     while(left <= right){
//         int mid = left + (right - left) / 2;
//         if(nums[mid] < target){
//             if(nums[mid] > nums[left])
//                 left = mid + 1;
//             else{ // nums[mid] < nums[left]
//                 if(nums[left] != target)
//                     left++;
//                 else
//                     return left;
//             } 
//         }

//         else if(nums[mid] == target)
//             return mid;

//         else{ // nums[mid] > target
//             if(nums[right] > nums[mid])
//                 right = mid - 1;
//             else{ // nums[right] < nums[mid]
//                 if(nums[right] != target)
//                     right--;
//                 else
//                     return right; 
//             }
//         }
//     }

//     return -1;
// }

// 法二：两次二分-开区间
// 153.寻找旋转排序数组中的最小值（返回的是下标）
int findMin(int* nums, int numsSize) { 
    int left = -1, right = numsSize - 1; // 开区间 (-1, n-1)
    while (left + 1 < right) { // 开区间不为空
        int mid = left + (right - left) / 2; // 不易越界
        if (nums[mid] < nums[numsSize - 1])
            right = mid;
        else  // nums[mid] >= nums[numsSize - 1]
            left = mid;
    } // ret=right
    // return nums[right];
    return right;
}

// 二分(有序数组中找target的下标)
int lowerBound(int* nums, int left, int right, int target){
    while(left + 1 < right){ // 开区间不为空
        // 循环不变量：
        // nums[right] >= target
        // nums[left] < target
        int mid = left + (right - left) / 2;
        if(nums[mid] >= target)
            right = mid;
        else
            left = mid;
    }

    return nums[right] == target ? right : -1; // 二分的返回值真正的意义是插入位置，所以在本题中还要判断最终插入位置上的元素是不是目标值target
}

int search(int* nums, int numsSize, int target) {
    int i = findMin(nums, numsSize);
    if(target > nums[numsSize - 1]) // target大于右半部分的最大值nums[numsSize-1]即target在左半部分
        return lowerBound(nums, -1, i, target); // 开区间(-1,i)
    return lowerBound(nums, i - 1, numsSize, target); // 开区间(i-1,numsSize)
}

// 法三：一次二分-开区间
```

