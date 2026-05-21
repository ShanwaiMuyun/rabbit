<script setup>
import { computed } from 'vue'
import { useRouter } from 'vue-router'
import { useUserStore } from '@/stores/userStore'

const userStore = useUserStore()
const router = useRouter()

const role = computed(() => userStore.userInfo?.role)

const confirm = () => {
    userStore.clearUserInfo()
    router.push('/login')
}
</script>

<template>
    <nav class="app-topnav">
        <div class="container">
            <ul>
                <template v-if="userStore.userInfo.token">
                    <li>
                        <a href="javascript:;">
                            <i class="iconfont icon-user"></i>{{ userStore.userInfo.username }}
                        </a>
                    </li>
                    <li>
                        <el-popconfirm
                            @confirm="confirm"
                            title="确认退出登录吗？"
                            confirm-button-text="确认"
                            cancel-button-text="取消"
                        >
                            <template #reference>
                                <a href="javascript:;">退出登录</a>
                            </template>
                        </el-popconfirm>
                    </li>
                    <li>
                        <a href="javascript:;" @click="$router.push('/orders')">我的订单</a>
                    </li>
                    <li>
                        <a href="javascript:;" @click="$router.push('/messages')">留言板</a>
                    </li>
                    <li v-if="role === 'seller'">
                        <a href="javascript:;" @click="$router.push('/seller/products')">商品管理</a>
                    </li>
                    <li v-if="role === 'admin'">
                        <a href="javascript:;" @click="$router.push('/admin/review')">审核管理</a>
                    </li>
                </template>
                <template v-else>
                    <li><a href="javascript:;" @click="$router.push('/login')">请先登录</a></li>
                    <li><a href="javascript:;" @click="$router.push('/register')">注册</a></li>
                    <li><a href="javascript:;" @click="$router.push('/messages')">留言板</a></li>
                    <li><a href="javascript:;">帮助中心</a></li>
                    <li><a href="javascript:;">关于我们</a></li>
                </template>
            </ul>
        </div>
    </nav>
</template>

<style scoped lang="scss">
.app-topnav {
    background: #333;

    ul {
        display: flex;
        height: 53px;
        justify-content: flex-end;
        align-items: center;

        li {
            a {
                padding: 0 15px;
                color: #cdcdcd;
                line-height: 1;
                display: inline-block;

                i {
                    font-size: 14px;
                    margin-right: 2px;
                }

                &:hover {
                    color: $xtxColor;
                }
            }

            ~li {
                a {
                    border-left: 2px solid #666;
                }
            }
        }
    }
}
</style>
