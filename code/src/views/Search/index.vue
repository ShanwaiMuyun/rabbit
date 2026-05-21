<script setup>
import { onMounted, ref, watch } from 'vue'
import { useRoute } from 'vue-router'
import { getProductsAPI } from '@/apis/product'
import GoodsItem from '@/views/Home/components/GoodsItem.vue'

const route = useRoute()
const goodsList = ref([])
const loading = ref(false)

const toGoods = (item) => ({
    id: item.id,
    name: item.name,
    desc: item.description,
    price: Number(item.price),
    picture: item.image_url,
    stock: Number(item.stock),
    seller: item.seller_username
})

const loadGoods = async () => {
    const keyword = route.query.keyword || ''
    loading.value = true
    try {
        const res = await getProductsAPI({ keyword })
        goodsList.value = (res.data || []).map(toGoods)
    } finally {
        loading.value = false
    }
}

onMounted(() => loadGoods())

watch(() => route.query.keyword, () => {
    loadGoods()
})
</script>

<template>
    <div class="search-page">
        <div class="container">
            <div class="page-head">
                <div>
                    <h2>商品搜索</h2>
                    <p>关键词：{{ route.query.keyword || '全部商品' }}</p>
                </div>
                <span>共 {{ goodsList.length }} 件商品</span>
            </div>

            <section class="panel" v-loading="loading">
                <ul v-if="goodsList.length" class="goods-list">
                    <li v-for="goods in goodsList" :key="goods.id">
                        <GoodsItem :goods="goods" />
                    </li>
                </ul>
                <el-empty v-else description="没有找到匹配的商品" />
            </section>
        </div>
    </div>
</template>

<style scoped lang="scss">
.search-page {
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

    p,
    span {
        color: #777;
        margin-top: 8px;
    }
}

.panel {
    min-height: 360px;
    background: #fff;
    padding: 24px;
    border-radius: 6px;
}

.goods-list {
    display: grid;
    grid-template-columns: repeat(4, 1fr);
    gap: 18px;

    li {
        height: 300px;
    }
}
</style>
