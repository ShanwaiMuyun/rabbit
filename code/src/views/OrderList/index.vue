<script setup>
import { computed, onMounted, ref } from 'vue'
import { ElMessage } from 'element-plus'
import { getOrdersAPI, receiveOrderAPI, shipOrderAPI } from '@/apis/order'
import { useUserStore } from '@/stores/userStore'

const userStore = useUserStore()
const orders = ref([])
const loading = ref(false)

const username = computed(() => userStore.userInfo?.username || '')
const role = computed(() => userStore.userInfo?.role === 'seller' ? 'seller' : 'buyer')

const statusMap = {
    ordered: { text: '已下单，待发货', type: 'warning' },
    shipped: { text: '已发货，待收货', type: 'primary' },
    received: { text: '已收货，交易完成', type: 'success' }
}

const loadOrders = async () => {
    if (!username.value) {
        return
    }

    loading.value = true
    try {
        const res = await getOrdersAPI({
            username: username.value,
            role: role.value
        })
        orders.value = res.data || []
    } finally {
        loading.value = false
    }
}

const shipOrder = async (row) => {
    await shipOrderAPI({
        orderId: row.id,
        seller: username.value
    })
    ElMessage.success('订单已发货')
    loadOrders()
}

const receiveOrder = async (row) => {
    await receiveOrderAPI({
        orderId: row.id,
        buyer: username.value
    })
    ElMessage.success('已确认收货，交易完成')
    loadOrders()
}

onMounted(() => loadOrders())
</script>

<template>
    <div class="orders-page">
        <div class="container">
            <div class="page-head">
                <div>
                    <h2>我的订单</h2>
                    <p>{{ role === 'seller' ? '处理买家订单和发货状态' : '查看购物订单并确认收货' }}</p>
                </div>
                <el-button type="primary" @click="loadOrders">刷新</el-button>
            </div>

            <section class="panel">
                <el-table v-loading="loading" :data="orders" border>
                    <el-table-column prop="id" label="订单号" width="90" />
                    <el-table-column label="商品" min-width="240">
                        <template #default="{ row }">
                            <div class="goods-cell">
                                <img :src="row.image_url" alt="" />
                                <div>
                                    <p>{{ row.product_name }}</p>
                                    <span>{{ role === 'seller' ? `买家：${row.buyer_username}` : `卖家：${row.seller_username}` }}</span>
                                </div>
                            </div>
                        </template>
                    </el-table-column>
                    <el-table-column prop="quantity" label="数量" width="80" />
                    <el-table-column label="金额" width="120">
                        <template #default="{ row }">
                            &yen;{{ Number(row.total_price).toFixed(2) }}
                        </template>
                    </el-table-column>
                    <el-table-column prop="address" label="收货地址" min-width="220" />
                    <el-table-column label="状态" width="160">
                        <template #default="{ row }">
                            <el-tag :type="statusMap[row.status]?.type">
                                {{ statusMap[row.status]?.text || row.status }}
                            </el-tag>
                        </template>
                    </el-table-column>
                    <el-table-column label="操作" width="150">
                        <template #default="{ row }">
                            <el-button
                                v-if="role === 'seller' && row.status === 'ordered'"
                                size="small"
                                type="primary"
                                @click="shipOrder(row)"
                            >
                                发货
                            </el-button>
                            <el-button
                                v-else-if="role === 'buyer' && row.status === 'shipped'"
                                size="small"
                                type="success"
                                @click="receiveOrder(row)"
                            >
                                确认收货
                            </el-button>
                            <span v-else class="muted">无可用操作</span>
                        </template>
                    </el-table-column>
                </el-table>
                <el-empty v-if="!loading && orders.length === 0" description="暂无订单" />
            </section>
        </div>
    </div>
</template>

<style scoped lang="scss">
.orders-page {
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

.panel {
    background: #fff;
    padding: 24px;
    border-radius: 6px;
}

.goods-cell {
    display: flex;
    align-items: center;

    img {
        width: 64px;
        height: 64px;
        object-fit: cover;
        border-radius: 4px;
        margin-right: 12px;
    }

    p {
        font-size: 15px;
    }

    span {
        display: inline-block;
        color: #888;
        margin-top: 6px;
    }
}

.muted {
    color: #999;
}
</style>
