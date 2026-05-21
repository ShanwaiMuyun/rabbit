<script setup>
import { computed, onMounted, ref } from 'vue'
import { ElMessage } from 'element-plus'
import { addMessageAPI, getMessagesAPI } from '@/apis/user'
import { useUserStore } from '@/stores/userStore'

const userStore = useUserStore()
const messages = ref([])
const content = ref('')
const loading = ref(false)
const submitting = ref(false)

const username = computed(() => userStore.userInfo?.username || '')
const isLogin = computed(() => Boolean(userStore.userInfo?.token))

const loadMessages = async () => {
    loading.value = true
    try {
        const res = await getMessagesAPI()
        messages.value = res.data || []
    } finally {
        loading.value = false
    }
}

const submitMessage = async () => {
    const value = content.value.trim()
    if (!isLogin.value) {
        ElMessage.warning('请先登录后再留言')
        return
    }
    if (!value) {
        ElMessage.warning('请输入留言内容')
        return
    }

    submitting.value = true
    try {
        await addMessageAPI({
            username: username.value,
            content: value
        })
        ElMessage.success('留言成功')
        content.value = ''
        loadMessages()
    } finally {
        submitting.value = false
    }
}

onMounted(() => loadMessages())
</script>

<template>
    <div class="messages-page">
        <div class="container">
            <div class="page-head">
                <div>
                    <h2>留言板</h2>
                    <p>用户可以在这里留下购物体验、商品建议或问题反馈</p>
                </div>
                <el-button @click="loadMessages">刷新</el-button>
            </div>

            <section class="message-editor">
                <el-alert
                    v-if="!isLogin"
                    title="当前未登录，可以浏览留言；登录后可以发布留言。"
                    type="info"
                    show-icon
                    :closable="false"
                />
                <el-input
                    v-model="content"
                    type="textarea"
                    :rows="4"
                    maxlength="300"
                    show-word-limit
                    placeholder="请输入留言内容"
                    :disabled="!isLogin"
                />
                <div class="editor-actions">
                    <span>{{ isLogin ? `当前用户：${username}` : '请先登录' }}</span>
                    <el-button type="primary" :loading="submitting" :disabled="!isLogin" @click="submitMessage">
                        发布留言
                    </el-button>
                </div>
            </section>

            <section class="message-list" v-loading="loading">
                <div v-if="messages.length" class="message-item" v-for="item in messages" :key="item.id">
                    <div class="avatar">{{ item.username?.slice(0, 1)?.toUpperCase() }}</div>
                    <div class="message-body">
                        <div class="meta">
                            <strong>{{ item.username }}</strong>
                            <span>{{ item.created_at }}</span>
                        </div>
                        <p>{{ item.content }}</p>
                    </div>
                </div>
                <el-empty v-else description="暂无留言" />
            </section>
        </div>
    </div>
</template>

<style scoped lang="scss">
.messages-page {
    padding: 24px 0 60px;
    min-height: 640px;
    background: #f5f5f5;
}

.page-head {
    height: 96px;
    display: flex;
    align-items: center;
    justify-content: space-between;
    background: #fff;
    padding: 0 24px;
    margin-bottom: 20px;
    border-radius: 6px;

    h2 {
        font-size: 24px;
        font-weight: 500;
    }

    p {
        color: #777;
        margin-top: 8px;
    }
}

.message-editor,
.message-list {
    background: #fff;
    padding: 24px;
    border-radius: 6px;
}

.message-editor {
    margin-bottom: 20px;

    .el-alert {
        margin-bottom: 16px;
    }
}

.editor-actions {
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-top: 16px;

    span {
        color: #777;
    }
}

.message-list {
    min-height: 260px;
}

.message-item {
    display: flex;
    padding: 18px 0;
    border-bottom: 1px solid #f0f0f0;

    &:last-child {
        border-bottom: 0;
    }
}

.avatar {
    width: 42px;
    height: 42px;
    border-radius: 50%;
    background: $xtxColor;
    color: #fff;
    display: flex;
    align-items: center;
    justify-content: center;
    margin-right: 14px;
    font-weight: 600;
}

.message-body {
    flex: 1;

    .meta {
        display: flex;
        align-items: center;
        justify-content: space-between;
        margin-bottom: 8px;

        strong {
            font-size: 16px;
        }

        span {
            color: #999;
        }
    }

    p {
        color: #555;
        line-height: 1.7;
        word-break: break-word;
    }
}
</style>
