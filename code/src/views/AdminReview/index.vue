<script setup>
import { computed, onMounted, ref } from 'vue'
import { ElMessage } from 'element-plus'
import { getReviewUsersAPI, reviewUserAPI, reviewProductAPI } from '@/apis/admin'
import { getProductsAPI } from '@/apis/product'
import { useUserStore } from '@/stores/userStore'

const userStore = useUserStore()
const users = ref([])
const products = ref([])
const loading = ref(false)

const adminName = computed(() => userStore.userInfo?.username || '')
const isAdmin = computed(() => userStore.userInfo?.role === 'admin')

const loadReviewData = async () => {
    if (!isAdmin.value) {
        return
    }

    loading.value = true
    try {
        const [userRes, productRes] = await Promise.all([
            getReviewUsersAPI({ admin: adminName.value, status: 'pending' }),
            getProductsAPI({ approvedOnly: 0 })
        ])

        users.value = userRes.data || []
        products.value = (productRes.data || []).filter((item) => item.status === 'pending')
    } finally {
        loading.value = false
    }
}

const reviewUser = async (userId, status) => {
    await reviewUserAPI({
        admin: adminName.value,
        userId,
        status
    })
    ElMessage.success(status === 'approved' ? '用户已通过审核' : '用户已拒绝')
    loadReviewData()
}

const reviewProduct = async (productId, status) => {
    await reviewProductAPI({
        admin: adminName.value,
        productId,
        status
    })
    ElMessage.success(status === 'approved' ? '商品已通过审核' : '商品已拒绝')
    loadReviewData()
}

onMounted(() => loadReviewData())
</script>

<template>
    <div class="review-page">
        <div class="container">
            <div class="page-head">
                <div>
                    <h2>审核管理</h2>
                    <p>处理新注册账号和卖家发布的待审核商品</p>
                </div>
                <el-button type="primary" @click="loadReviewData">刷新</el-button>
            </div>

            <el-alert
                v-if="!isAdmin"
                title="当前账号不是管理员，请使用 admin 账号登录后访问审核管理。"
                type="warning"
                show-icon
                :closable="false"
            />

            <template v-else>
                <section class="review-section">
                    <h3>待审核用户</h3>
                    <el-table v-loading="loading" :data="users" border>
                        <el-table-column prop="id" label="ID" width="80" />
                        <el-table-column prop="username" label="用户名" />
                        <el-table-column prop="role" label="角色" width="120">
                            <template #default="{ row }">
                                <el-tag :type="row.role === 'seller' ? 'warning' : 'success'">
                                    {{ row.role === 'seller' ? '卖家' : '买家' }}
                                </el-tag>
                            </template>
                        </el-table-column>
                        <el-table-column prop="created_at" label="注册时间" />
                        <el-table-column label="操作" width="180">
                            <template #default="{ row }">
                                <el-button size="small" type="primary" @click="reviewUser(row.id, 'approved')">通过</el-button>
                                <el-button size="small" type="danger" @click="reviewUser(row.id, 'rejected')">拒绝</el-button>
                            </template>
                        </el-table-column>
                    </el-table>
                    <el-empty v-if="!loading && users.length === 0" description="暂无待审核用户" />
                </section>

                <section class="review-section">
                    <h3>待审核商品</h3>
                    <el-table v-loading="loading" :data="products" border>
                        <el-table-column label="图片" width="100">
                            <template #default="{ row }">
                                <img class="product-img" :src="row.image_url" alt="" />
                            </template>
                        </el-table-column>
                        <el-table-column prop="name" label="商品名称" />
                        <el-table-column prop="seller_username" label="卖家" width="140" />
                        <el-table-column prop="price" label="价格" width="100" />
                        <el-table-column prop="stock" label="库存" width="100" />
                        <el-table-column label="操作" width="180">
                            <template #default="{ row }">
                                <el-button size="small" type="primary" @click="reviewProduct(row.id, 'approved')">通过</el-button>
                                <el-button size="small" type="danger" @click="reviewProduct(row.id, 'rejected')">拒绝</el-button>
                            </template>
                        </el-table-column>
                    </el-table>
                    <el-empty v-if="!loading && products.length === 0" description="暂无待审核商品" />
                </section>
            </template>
        </div>
    </div>
</template>

<style scoped lang="scss">
.review-page {
    padding: 24px 0 60px;
    background: #f5f5f5;
    min-height: 640px;
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

.review-section {
    background: #fff;
    padding: 20px 24px 28px;
    margin-top: 20px;
    border-radius: 6px;

    h3 {
        font-size: 18px;
        font-weight: 500;
        margin-bottom: 18px;
    }
}

.product-img {
    width: 64px;
    height: 64px;
    object-fit: cover;
    border-radius: 4px;
}
</style>
