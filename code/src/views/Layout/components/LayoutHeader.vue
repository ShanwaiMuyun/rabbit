<script setup>
import { ref } from 'vue'
import { useRouter } from 'vue-router'
import HeaderCart from './HeaderCart.vue'
import LayoutHeaderUl from './LayoutHeaderUl.vue'

const router = useRouter()
const keyword = ref('')

const doSearch = () => {
    const value = keyword.value.trim()
    if (!value) {
        return
    }

    router.push({
        path: '/search',
        query: {
            keyword: value
        }
    })
}
</script>

<template>
    <header class="app-header">
        <div class="container">
            <h1 class="logo">
                <RouterLink to="/">商城</RouterLink>
            </h1>
            <LayoutHeaderUl />
            <div class="search">
                <i class="iconfont icon-search" @click="doSearch"></i>
                <input
                    v-model="keyword"
                    type="text"
                    placeholder="搜索商品"
                    @keyup.enter="doSearch"
                >
            </div>
            <HeaderCart />
        </div>
    </header>
</template>

<style scoped lang="scss">
.app-header {
    background: #fff;

    .container {
        display: flex;
        align-items: center;
    }

    .logo {
        width: 200px;

        a {
            display: block;
            height: 132px;
            width: 100%;
            text-indent: -9999px;
            background: url('@/assets/images/logo.png') no-repeat center 18px / contain;
        }
    }

    .search {
        width: 190px;
        height: 32px;
        position: relative;
        border-bottom: 1px solid #e7e7e7;
        line-height: 32px;

        .icon-search {
            font-size: 18px;
            margin-left: 5px;
            cursor: pointer;
        }

        input {
            width: 150px;
            padding-left: 5px;
            color: #666;
        }
    }

    .cart {
        width: 50px;
    }
}
</style>
